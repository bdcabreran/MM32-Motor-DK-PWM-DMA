////////////////////////////////////////////////////////////////////////////////
/// @file     reg_opam.h
/// @author   AE TEAM
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE SERIES OF
///           MM32 FIRMWARE LIBRARY.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion

#ifndef __REG_OPAM_H
#define __REG_OPAM_H

// Files includes

#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "reg_common.h"




#if defined ( __CC_ARM )
#pragma anon_unions
#endif








////////////////////////////////////////////////////////////////////////////////
/// @brief OPAMP Base Address Definition
////////////////////////////////////////////////////////////////////////////////
#define OPAMP_BASE                  (APB2PERIPH_BASE + 0x3C10)              ///< Base Address: 0x40013C10



////////////////////////////////////////////////////////////////////////////////
/// @brief Operational Amplifier Registers Structure Definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    __IO u32 CSR;                                                               ///< OPAMP Control Status Register                  Offset: 0x00
} OPAMP_TypeDef;



////////////////////////////////////////////////////////////////////////////////
/// @brief OPA type pointer Definition
////////////////////////////////////////////////////////////////////////////////
#define OPAMP                           ((OPAMP_TypeDef*) OPAMP_BASE)




////////////////////////////////////////////////////////////////////////////////
/// @brief OPAMP_CSR Register Bit Definition
////////////////////////////////////////////////////////////////////////////////
#define OPAMP_CSR_OP1_Pos           (0)
#define OPAMP_CSR_OP1               (0x01U << OPAMP_CSR_OP1_Pos)            ///< operational amplifier one enable

#define OPAMP_CSR_OP2_Pos           (8)
#define OPAMP_CSR_OP2               (0x01U << OPAMP_CSR_OP2_Pos)            ///< operational amplifier two enable

#define OPAMP_CSR_OP3_Pos           (16)
#define OPAMP_CSR_OP3               (0x01U << OPAMP_CSR_OP3_Pos)            ///< operational amplifier three enable

#define OPAMP_CSR_OP4_Pos           (24)
#define OPAMP_CSR_OP4               (0x01U << OPAMP_CSR_OP4_Pos)            ///< operational amplifier four enable




/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
