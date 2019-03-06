# Class SimionMemPool
> Source: mem-pool.cpp
## Methods
### ``void initialize(MemBlock* pBlock)``
* **Summary**
  This method resets each interleaved buffer within a MemBlock to its initial value
* **Parameters**
  * _pBlock_: Memory block
### ``void init(BUFFER_SIZE blockSize)``
* **Summary**
  After adding all the requested buffers, this method initializes all the necessary data so that several MemBlocks are allocated
* **Parameters**
  * _blockSize_: Desired MemBlock size (element count)
### ``void copy(IMemBuffer* pSrc, IMemBuffer* pDst)``
* **Summary**
  Copies data from one buffer to another. They must belong to the same MemPool
* **Parameters**
  * _pSrc_: Source buffer
  * _pDst_: Destination buffer
