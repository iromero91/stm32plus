/*
 * This file is a part of the open source stm32plus library.
 * Copyright (c) 2011,2012,2013 Andy Brown <www.andybrown.me.uk>
 * Please see website for licensing terms.
 */

#include "config/stm32plus.h"
#include "config/dma.h"


// this is only for the F1

#ifdef STM32PLUS_F1

using namespace stm32plus;


// static initialisers for the hack that forces the IRQ handlers to be linked

template<> DmaInterruptFeatureEnabler<1,4>::FPTR DmaInterruptFeatureEnabler<1,4>::_forceLinkage=nullptr;


extern "C" {

/**
 * DMA1, channel 4
 */

#if defined(USE_DMA1_4_INTERRUPT)
  void __attribute__ ((interrupt("IRQ"))) DMA1_Channel4_IRQHandler() {

    if(DMA_GetITStatus(DMA1_IT_TC4)!=RESET) {
      DmaInterruptFeature<1,4>::_dmaInstance->DmaInterruptEventSender.raiseEvent(DmaEventType::EVENT_COMPLETE);
      DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
    else if(DMA_GetITStatus(DMA1_IT_HT4)!=RESET) {
      DmaInterruptFeature<1,4>::_dmaInstance->DmaInterruptEventSender.raiseEvent(DmaEventType::EVENT_HALF_COMPLETE);
      DMA_ClearITPendingBit(DMA1_IT_HT4);
    }
    else if(DMA_GetITStatus(DMA1_IT_TE4)!=RESET) {
      DmaInterruptFeature<1,4>::_dmaInstance->DmaInterruptEventSender.raiseEvent(DmaEventType::EVENT_TRANSFER_ERROR);
      DMA_ClearITPendingBit(DMA1_IT_TE4);
    }
    __DSB();      // prevent erroneous recall of this handler due to delayed memory write
  }
#endif

}
#endif // STM32PLUS_F1
