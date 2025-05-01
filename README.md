# EC513 Spring 2025 Project
**All changed files are under "gem5/src/mem/cache/replacement_policies/" directory.**

The changes are as follows:


- base.hh
    - Added an override for the touch function that takes in a PacketPtr so that we can log if a memory access is a read or write. 
- replaceable_entry.hh
    - ReplacementData struct now has an isWrite boolean field to track access type. 
- lfu_rp.hh (our implementation is in lfu_rp.cc)
    - Added lastTouchTick to LFUReplData. This allows us to combine the LFU and LRU concepts. 
- lfu_rp.cc
    - Invalidate Function
        - Set lastTouchTick to 0
        - set refCount to 1 
    - Touch Function
        - Update lastTouchTick
        - If the access is a write, we update the refCount += 1, otherwise we set refCount = 1.
    - Reset Function
        - set refCount = 1
        - set lastTouchTick to current tick
    - getVicitim Function
        - We search through all the candidates to find the two least recently used blocks.
        - Between these two blocks, we see which is least frequently referenced and set that one to be the victim. The one that is saved has its reference count set to 1. 
