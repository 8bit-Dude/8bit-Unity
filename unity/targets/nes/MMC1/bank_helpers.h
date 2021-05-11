// Contains functions to help with working with multiple PRG/CHR banks

// Maximum level of recursion to allow with banked_call and similar functions.
#define MAX_BANK_DEPTH 10


// Switch to another bank and call this function.
// Note: Using banked_call to call a second function from within  
// another banked_call is safe. This will break if you nest more  
// than 10 calls deep.
void banked_call(unsigned char bankId, void (*method)(void));


// Internal function used by banked_call(), don't call this directly.
// Switch to the given bank, and keep track of the current bank, so 
// that we may jump back to it as needed.
void bank_push(unsigned char bankId);


// Internal function used by banked_call(), don't call this directly.
// Go back to the last bank pushed on using bank_push.
void bank_pop(void);


// Switch to the given bank (to $8000-bfff). Your prior bank is not saved.
// Can be used for reading data with a function in the fixed bank.
// bank_id: The bank to switch to.
void __fastcall__ set_prg_bank(unsigned char bank_id);


// Get the current PRG bank at $8000-bfff.
// returns: The current bank.
unsigned char __fastcall__ get_prg_bank(void);


// Set the current 1st 4k chr bank to the bank with this id.
void __fastcall__ set_chr_bank_0(unsigned char bank_id);


// Set the current 2nd 4k chr bank to the bank with this id.
void __fastcall__ set_chr_bank_1(unsigned char bank_id);


#define MIRROR_LOWER_BANK 0
#define MIRROR_UPPER_BANK 1
#define MIRROR_VERTICAL 2
#define MIRROR_HORIZONTAL 3

// Set the current mirroring mode. Your options are MIRROR_LOWER_BANK, 
// MIRROR_UPPER_BANK, MIRROR_HORIZONTAL, and MIRROR_VERTICAL.
void __fastcall__ set_mirroring(unsigned char mirroring);


// Set all 5 bits of the $8000 MMC1 Control register (not recommended)
void __fastcall__ set_mmc1_ctrl(unsigned char value);


// Set what chr bank to set at the top of the screen, for a split screen.
void __fastcall__ set_nmi_chr_tile_bank(unsigned char bank);


// Don't change the chr bank at the top of the screen.
void __fastcall__ unset_nmi_chr_tile_bank(void);



// some things deleted


