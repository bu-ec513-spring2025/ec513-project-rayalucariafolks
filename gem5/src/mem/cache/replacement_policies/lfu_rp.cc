/**
 * Copyright (c) 2018-2020 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mem/cache/replacement_policies/lfu_rp.hh"

#include <cassert>
#include <memory>

#include "params/LFURP.hh"

namespace gem5
{

namespace replacement_policy
{

LFU::LFU(const Params &p)
  : Base(p)
{
}

void
LFU::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    // Reset reference count
    std::static_pointer_cast<LFUReplData>(replacement_data)->refCount = 1; // it was one in paper
    // Reset last touch timestamp
    std::static_pointer_cast<LFUReplData>(
        replacement_data)->lastTouchTick = Tick(0);
}

void
LFU::touch(const std::shared_ptr<ReplacementData>& replacement_data) const // HIT
{
    // Update last touch timestamp
    // if (pkt->isRead()){
    //         std::static_pointer_cast<LFUReplData>(
    //     replacement_data)->lastTouchTick = Tick(0);
    // }

     // Update last touch timestamp
    std::static_pointer_cast<LFUReplData>(
        replacement_data)->lastTouchTick = curTick();

    if (replacement_data->isWrite){
        std::static_pointer_cast<LFUReplData>(replacement_data)->refCount+=1;
    }else{
        std::static_pointer_cast<LFUReplData>(replacement_data)->refCount=1;
    }
    
}

void
LFU::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Reset reference count
    std::static_pointer_cast<LFUReplData>(replacement_data)->refCount = 1;
    // Set last touch timestamp
    std::static_pointer_cast<LFUReplData>(
        replacement_data)->lastTouchTick = curTick();
}

ReplaceableEntry*
LFU::getVictim(const ReplacementCandidates& candidates) const // MISS
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);
    ReplaceableEntry* victim = candidates[0];
    if(candidates.size()>1){
        
    // Visit all candidates to find victim
    ReplaceableEntry* victim0 = candidates[0];
    ReplaceableEntry* victim1 = candidates[1];
    for (const auto& candidate : candidates) {
        // Update victim entry if necessary
        if (std::static_pointer_cast<LFUReplData>(
                    candidate->replacementData)->lastTouchTick <
                std::static_pointer_cast<LFUReplData>(
                    victim0->replacementData)->lastTouchTick) {
            victim0 = candidate;
        }
    }
   
    for (const auto& candidate : candidates) {
        // Update victim entry if necessary
        if (std::static_pointer_cast<LFUReplData>(
                    candidate->replacementData)->lastTouchTick <
                std::static_pointer_cast<LFUReplData>(
                    victim1->replacementData)->lastTouchTick) {
                        if(std::static_pointer_cast<LFUReplData>(
                    victim0->replacementData)->lastTouchTick < std::static_pointer_cast<LFUReplData>(
                    candidate->replacementData)->lastTouchTick ){
                            victim1 = candidate;
                        }
        }
    }
    if(std::static_pointer_cast<LFUReplData>(
                    victim1->replacementData)->refCount > std::static_pointer_cast<LFUReplData>(
                    victim0->replacementData)->refCount ){
            std::static_pointer_cast<LFUReplData>(
                    victim1->replacementData)->refCount=1;
                    victim=victim0;

    }else{
        victim=victim1;
        std::static_pointer_cast<LFUReplData>(
                    victim0->replacementData)->refCount=1;
    }


    }
    return victim;
}

std::shared_ptr<ReplacementData>
LFU::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new LFUReplData());
}

} // namespace replacement_policy
} // namespace gem5
