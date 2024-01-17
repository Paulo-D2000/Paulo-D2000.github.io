---
layout: post
title:  "C++ GMSK Modem Series - Part 1"
date:   2024-1-16 21:15:00 -0300
author: Paulo Dutra
categories:
  - en-US
tags:
  - modem
  - ccsds
  - gmsk
  - threads
  - cpp
  - dsp
  - iq
  - sdr
---

<h1>The Begining...</h1>

This is the first part of my adventure trying to implement one GMSK modem in C++, note that it's still under development and probably a lot of things will be changing between posts as I refactor some of the code and find more bugs (there are many, LOL).

GMSK (Gaussian Minimum Shift Keying) is interesting because it can be modulated and demodulated (non-coherently) like FSK (Frequency Shift Keying), this simplifies a lot of processing steps modulate and demodulate the IQ signal. Another benefit is that it has a lower BER (Bit Error Rate) than FSK and uses a simpler Gaussian Filter instead of the root raised-cosine-matched filter usually used with digital modulations.

Some results plotted on GnuRadio (Scaling may vary between the plots...):

![gmsk_results_20](/assets/img/gmsk_results_20db.jpg)\
GMSK Modem with 0.5 BTb, 1200bd bitrate, 40x Oversampling simulated with 20dB SNR. 

![gmsk_results_30](/assets/img/gmsk_results_30db.jpg)\
GMSK Modem with 0.5 BTb, 1200bd bitrate, 40x Oversampling simulated with 30dB SNR.

<h3> The Architecture </h3>

I wanted to "Pipeline" the stages of the Modem in a way similar to how [GnuRadio][1] and some other SDR software work by having each step (Filtering, FM Modulation, etc) in a separate "Block" with a worker thread running concurrently. This is a bit tricky to implement and I'm probably not doing it right, tips are welcome and you can find the code on [GitHub](https://github.com/Paulo-D2000/PopoSDR).

The blocks are derived from the base `Block<IT, OT>` class and override the virtual `work(...)` function, `IT` is the input type and `OT` is the output type. Each Block has pointers to `Stream<T>` objects each one holding one Circular `Buffer<T>` object and some thread synchronization logic.

Here's a simplified view of the Block class:

```cpp
template<typename IT, typename OT>
class Block<IT, OT>
{
public:
  Block(const size_t& BufferSize){...}

  void start(){...}

  virtual size_t work(size_t const size_t& n_inputItems, std::vector<IT>&  input, std::vector<OT>& output) {...}

  void stop(){...}

  template <typename X>
  void connect(Block<X,IT>& Other)

private:
  Stream<IT>* InputStream;
  Stream<OT>* OutputOutput;
  std::thread worker;
};
```

The code snippet above shows one simplified view of the Block class, you can see that we have some functions:

### `Block(const& size_t BufferSize)`
This is the constructor of the class, it receives a `const size_t&`, basically one very big unsigned integer, `BufferSize` that is used to allocate the input and output stream buffers.

### `start()`
This function is used to start the block's internal `Stream<T>` objects, it also starts the `worker` thread that calls the `work` function in a loop feeding it with new samples from the input Stream and forwarding them to the output Stream.

### `stop()`
As you've guessed, this function stops the Streams and joins the worker thread. The thread waits for some conditions to be true before exiting, perhaps this could be improved by having one `force` parameter that forces the thread to exit...

### `connect(Block<...>& Other)`
Connect is used to link 2 Blocks by setting the InputStream pointer of the current Block (`this`) to the `Other` Block's OutputStream pointer. Looking at the code snippet you will see that `Other`'s input type isn't defined, while the output type must be the same as `this` input type.

The usage is something like:
```cpp
Block<IT, float> A(1024); // A has IT type input and float output
Block<float, T> B(1024); // B has float type input and T output
B.connect(A); // Connection (float) B <- A
```

<h3> The Problem </h3>

When using multiple threads that access and modify a shared resource, the `Buffer<T>` objects in this case, we must synchronize the reads and writes making sure threads don't run into Race Conditions and DeadLocks. Race Conditions happen when 2 threads try to modify the same resource for example, Thread-0 tries to write to the buffer while, at the same time, Thread-1 tries to read from the buffer causing undefined behaviour... DeadLocks happen when 2 co-dependent threads are waiting for some shared resource to be free.

Ideally, you would use one lock-free and wait-free atomic-based circular buffer to get the maximum throughput without waiting on condition variables, locking and unlocking mutexes, etc. This would add some problems to the current state of the code since the threads use condition variables to wait for the input to be filled with data. I will discuss how I decided to implement this on the following topic.

<h3> One possible Solution </h3>

After a lot of searching online, and swearing with ChatGPT LOL, I found some solutions that partially worked, I've ended up using the "Channel" or "Stream" as I call it approach. This was based on an [answer from StackOverflow][2]. The key idea is having one shared `Stream<T>` object that holds a `condition_variable`, one `mutex`, and the `Buffer<T>` object. 

The mutex and condition_variable are used to synchronize the 2 threads that share the Stream, while the access to the buffer is done via `writeToBuffer()` and `readFromBuffer()` functions that receive a reference to one `vector<T>` object used to hold the data that will be passed to and from the work function and a `size_t` variable representing the number of items to be read or written to the data vector. There are also `open()` and `close()` functions that are mostly used to signal the Block threads if the stream is available or not, this is used to terminate the threads when the stream is empty and closed.

Simplified code showing the `Stream<T>` class:

```cpp
template <typename T>
class Stream {

private:
  Buffer<T> buffer;
  std::mutex m_mtx;
  std::condition_variable m_cv;

public:
  Stream(const size_t& Buffersize) {...}

  void open() {...}

  void close() {...}

  void writeToBuffer(const std::vector<T>& data, size_t N) {...}

  size_t readFromBuffer(std::vector<T>& data, size_t N) {...}
};
```

### `writeToBuffer()`
The function acquires one lock to the mutex and writes N items from the data vector on the internal Buffer object, after that it notifies the condition_variable and releases the lock. The next thread is waiting on this condition_variable and starts reading data after it is notified.

### `readFromBuffer()`
The function acquires one lock to the mutex and tries to read N items from the internal Buffer object on the data vector, after that it notifies the condition_variable and releases the lock, the return value is the number of items read. The thread waits on the condition_variable until the input isn't empty and starts processing data after it is notified.

### `Stream(const& size_t BufferSize)`
The Stream constructor also receives one `size_t` variable forwarded to the internal `Buffer<T>` object, about the Buffers they're also a very basic circular buffer implementation with unsynchronized `read()` and `write()` methods that push and pull 1 item per call wrapping the read and write indices when they're greater than the buffer size. 

The main advantage of circular buffers is that the 2 threads accessing the buffer can write and read sequentially much like one Queue or FIFO object the difference being that after the buffer is full it just overwrites itself, the empty state happens when the reading index is the same as the writing index, meaning all inputs were consumed.

On the snippet below is a simplified overview of the `Buffer<T>` class:

```cpp
template <typename T>
class Buffer
{
public:
  Buffer();

  Buffer(const size_t& BufferSize);

  void resize(const size_t& BufferSize);

  void write(const T& sample);

  T read();

  bool isEmpty(){
    return m_head == m_tail;
  }

  bool isFull(){
    return ((m_head+1) & m_mask) == m_tail;
  }

  ~Buffer();

private:
  size_t m_head;
  size_t m_tail;

  size_t m_size;
  size_t m_mask;

  std::unique_ptr<T> m_buffer;
  size_t m_occupancy;
};
```

I will just overview the details of this class since many ring buffer implementations are available online and this one doesn't have anything special, it's even a bit suboptimal.

### `Buffer()`
This is a default constructor and I'm not sure it's even needed,
it just sets everything to zero.

### `Buffer(const size_t& BufferSize)`
The main constructor of the class where the unique_ptr<T> that holds the data is allocated and the indices are set to zero, the size must be a power of 2 so it logs one error and resizes the buffer to the closest power of 2. This isn't the most memory-efficient alternative but it simplifies the wrapping around logic by using a bitwise `and &` operation instead of integer modulo `%`.

### `resize(const size_t& BufferSize)`
This function was previously used to resize the buffer that was created using the default constructor, it will be probably removed later...

### `write(const T& sample)`
This writes one item to the buffer and will log one error overwriting the buffer when it's full.

### `read()`
Like `write()` it reads an item from the buffer and logs one error returning an empty `T()` if the buffer is empty.

### `isEmpty()` and `isFull()`
Are used to check if the buffer is empty or full by checking the read and write indices, they return a `bool` set to `true` if any of the conditions were met.

### `~Buffer()`
The destructor mainly just logs a debug string showing that the buffer was destroyed.

<h3> The current Output Format </h3>

I'm using `WAVE` files `.wav` to output the modulator IQ data (2 channel wav file) and some of the demodulator outputs like the `FM Demod`, and `TimingPLL` block outputs. No libraries were used for this since the uncompressed 16bit PCM `WAVE` format is pretty simple, it contains one header, implemented as one struct, and one array of `int16` representing the signed data.

There isn't anything special about the writing of the files and I will show one snippet below on how it's implemented... I've added some logic to detect clipping, that is, samples being higher than the maximum level, and correcting for it.

```cpp
struct WavHeader{
  #pragma pack(push, 1) // Compiler directive -> no padding between the variables...
  /* RIFF CHUNK */
  U32 RIFF_ID = 0x46464952;  // "RIFF"
  U32 RIFF_SZ = 0x00000000;  // FileSize - 8
  U32 RIFF_TY = 0x45564157;  // "WAVE"
  /* FMT CHUNK*/
  U32 FMT_ID = 0x20746D66;   // "fmt"
  U32 FMT_SZ = 16;           // Chunk size
  U16 ComprCode = 1;         // Compression Code
  U16 NChans = 1;            // NChannels
  U32 SampleRate = 48000;    // Sample Rate
  U32 ByteRate = 96000;      // Byte Rate = SampleRate * BlockAlign
  U16 BlockAlign = 2;        // BlockAlign = NChannels * BitsDepth/8
  U16 BitDepth = 16;         // BitDepth
  /* DATA CHUNK */
  U32 DATA_ID = 0x61746164;  // "data"
  U32 DataSize = 0x00000000; // Data size
  /* PCM DATA */
  #pragma pack(pop)
};

void WriteWav(std::string filename, std::vector<F32> data, size_t SampleRate=48000, int Channels=1, float scale=1.0f){
  LOG_INFO("Writing Wav");
  LOG_TEST("Path: {}",filename);
  LOG_TEST("SampleRate: {}",SampleRate);
  LOG_TEST("Channels: {}",Channels);
  LOG_TEST("Data Size: {}",data.size());

  /* File gets closed when std::ofstream goes out of scope */ 
  std::ofstream outFile(filename, std::ios::binary);

  /* Wav header (16 Bit Signed PCM Samples) */
  WavHeader hdr;
  hdr.NChans = Channels;
  hdr.SampleRate = (U32)SampleRate;
  hdr.BitDepth = 8 * sizeof(I16);
  hdr.BlockAlign = Channels * hdr.BitDepth/8;
  hdr.ByteRate = hdr.SampleRate * hdr.BlockAlign;
  hdr.DataSize = (U32)data.size() * hdr.BitDepth/8;
  hdr.RIFF_SZ = hdr.DataSize + sizeof(hdr) - 8;

  // Write Header (44 bytes)
  outFile.write((char*)&hdr, sizeof(hdr)); 

  size_t nclip = 0;

  // Write Data
  for (size_t i = 0; i < data.size(); i++)
  {
    F32 Isample = data.at(i) * scale * (float)INT16_MAX;
    if(std::abs(Isample) > (float)INT16_MAX){
      Isample *= 1.0f/Isample;
      nclip++;
    }
    I16 sample = (int)(Isample); // Convert FP32 -> S16
    outFile.write((char*)&sample, sizeof(I16)); // Write S16 as 2 char's
  }
  if(nclip > 0){
    LOG_ERROR("Clipping {} samples...",nclip);
  }
}
```

Most of the code is the `WriteWav` function, it writes the wave file and receives a vector of floats, `F32` alias, with the data already interleaved in the case of multiple channels. The `WavHeader` struct is mostly composed of constants following the [specification][3].

I've also made one simple logging "library", more of one header, with some macros that print info on the terminal using C++20's std::format. The available macros are `LOG_INFO`, `LOG_DEBUG`, `LOG_TEST`, and `LOG_ERROR` which print to stdout. Each macro receives a format string and its args via the `VA_ARGS` macro argument and prints with a different color using ASCII escape codes, there's one example log below.

<img src="{{ site.baseurl }}/assets/img/log_test.jpg" width="250" height="192" />

<h3> In the next post... </h3>

In the next post, I will show how the modulator was implemented, diving into the code of the blocks! There will also be a more detailed explanation of the GMSK modulation and some of the design choices of this modem. I originally wanted to have everything on a single C++ file, but splitting the modem into one main file and a library seems more flexible as the blocks can be used to make other kinds of modems...

Here's one preview of the next post's content...

```
     Bytes
       ↓
      Bits
       ↓
 CCSDS Scramble
       ↓
    Repeat
       ↓
Gaussian Filter
       ↓
 FM Modulation
       ↓
 Interpolation
       ↓
   Wav output
```

```cpp
class Modulator
{
public:
  Modulator(size_t BaudRate=1200, size_t SampleRate=48000, size_t BufferSize=4096);

  void start(){...}

  void sendPacket(...) {...}

  void modulate(Stream<U8>& input_stream, bool scramble=false) {...}

  void stop(){...}

private:
  Stream<F32> m_inp_stream;
  Stream<CF32>* m_out_stream;
  FirFilter<F32>* m_Gfilter;
  FirFilter<CF32>* m_interpolator;
  FmModulator* m_fmmod;
};
```

Thanks for reading! I hope you liked the content and will keep reading the next posts of this series. Bye!

[1]: <https://gnuradio.org>
[2]: <https://stackoverflow.com/questions/68055538/how-to-share-mutex-condition-variable-and-queue-between-two-classes-c>
[3]: <https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html>