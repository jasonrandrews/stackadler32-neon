#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arm_neon.h>

// The prime value used in Adler-32 algorithm
const uint32_t ADLER32_PRIME = 65521;

// C implementation of Adler-32 Update function with ARM NEON optimization
void adler32_update(uint32_t *s1, uint32_t *s2, const unsigned char *buf, size_t len) {
    uint32_t a = *s1;
    uint32_t b = *s2;
    
    // Handle empty buffer case
    if (len == 0) {
        *s1 = 1;
        *s2 = 0;
        return;
    }
    
    // For small buffers, use the standard implementation
    if (len < 16) {
        for (size_t n = 0; n < len; n++) {
            a = (a + buf[n]) % ADLER32_PRIME;
            b = (b + a) % ADLER32_PRIME;
        }
        *s1 = a;
        *s2 = b;
        return;
    }
    
    // Process data in blocks to avoid too frequent modulo operations
    const size_t BLOCK_SIZE = 5552; // Max size before s1 can overflow
    size_t blocks = len / BLOCK_SIZE;
    size_t remaining = len % BLOCK_SIZE;
    
    // Process each block
    for (size_t block = 0; block < blocks; ++block) {
        const unsigned char* block_data = buf + (block * BLOCK_SIZE);
        
        // Process this block in chunks of 16 bytes
        for (size_t i = 0; i < BLOCK_SIZE; i += 16) {
            // Load 16 bytes
            uint8x16_t v_data = vld1q_u8(block_data + i);
            
            // Extract bytes and update a and b
            a += vgetq_lane_u8(v_data, 0);
            b += a;
            a += vgetq_lane_u8(v_data, 1);
            b += a;
            a += vgetq_lane_u8(v_data, 2);
            b += a;
            a += vgetq_lane_u8(v_data, 3);
            b += a;
            a += vgetq_lane_u8(v_data, 4);
            b += a;
            a += vgetq_lane_u8(v_data, 5);
            b += a;
            a += vgetq_lane_u8(v_data, 6);
            b += a;
            a += vgetq_lane_u8(v_data, 7);
            b += a;
            a += vgetq_lane_u8(v_data, 8);
            b += a;
            a += vgetq_lane_u8(v_data, 9);
            b += a;
            a += vgetq_lane_u8(v_data, 10);
            b += a;
            a += vgetq_lane_u8(v_data, 11);
            b += a;
            a += vgetq_lane_u8(v_data, 12);
            b += a;
            a += vgetq_lane_u8(v_data, 13);
            b += a;
            a += vgetq_lane_u8(v_data, 14);
            b += a;
            a += vgetq_lane_u8(v_data, 15);
            b += a;
        }
        
        // Apply modulo after processing the block
        a %= ADLER32_PRIME;
        b %= ADLER32_PRIME;
    }
    
    // Process remaining bytes
    const unsigned char* remaining_data = buf + (blocks * BLOCK_SIZE);
    
    // Process remaining bytes in chunks of 16
    size_t i = 0;
    for (; i + 16 <= remaining; i += 16) {
        // Load 16 bytes
        uint8x16_t v_data = vld1q_u8(remaining_data + i);
        
        // Extract bytes and update a and b
        a += vgetq_lane_u8(v_data, 0);
        b += a;
        a += vgetq_lane_u8(v_data, 1);
        b += a;
        a += vgetq_lane_u8(v_data, 2);
        b += a;
        a += vgetq_lane_u8(v_data, 3);
        b += a;
        a += vgetq_lane_u8(v_data, 4);
        b += a;
        a += vgetq_lane_u8(v_data, 5);
        b += a;
        a += vgetq_lane_u8(v_data, 6);
        b += a;
        a += vgetq_lane_u8(v_data, 7);
        b += a;
        a += vgetq_lane_u8(v_data, 8);
        b += a;
        a += vgetq_lane_u8(v_data, 9);
        b += a;
        a += vgetq_lane_u8(v_data, 10);
        b += a;
        a += vgetq_lane_u8(v_data, 11);
        b += a;
        a += vgetq_lane_u8(v_data, 12);
        b += a;
        a += vgetq_lane_u8(v_data, 13);
        b += a;
        a += vgetq_lane_u8(v_data, 14);
        b += a;
        a += vgetq_lane_u8(v_data, 15);
        b += a;
    }
    
    // Process any final bytes
    for (; i < remaining; ++i) {
        a += remaining_data[i];
        b += a;
    }
    
    // Final modulo
    a %= ADLER32_PRIME;
    b %= ADLER32_PRIME;
    
    *s1 = a;
    *s2 = b;
}
