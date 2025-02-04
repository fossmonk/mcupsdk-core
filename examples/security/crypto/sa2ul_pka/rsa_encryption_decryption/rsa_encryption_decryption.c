/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This example demonstrates the PKA Encryption Decryption operations */

#include <string.h>
#include <kernel/dpl/DebugP.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

static uint32_t gPkaRsaOutputResult[RSA_MAX_LENGTH + 1];

/* Sa2ul Pka Instance */
#define SA2UL_PKA_INSTANCE				(0U)

/* Openssl command To generate public key: Openssl rsa -pubout -in private.pem -out public.pem
The below key is in Bigint format please check in Api guide to know about Bigint format*/
static const struct AsymCrypt_RSAPubkey gPkaRsaPublicKey =
{
	{
		128UL,
		0x7b0e1331UL, 0x21d93fafUL, 0xf24ace7cUL, 0x2e1c2805UL,
		0x2505b8eaUL, 0x789f6d3bUL, 0x1429332dUL, 0x410ebcb3UL,
		0x11765fc0UL, 0xa37281bcUL, 0xdcb72cd4UL, 0x057f8582UL,
		0xfbc664dbUL, 0x1c68e56dUL, 0x483dfb84UL, 0x01d8ce5fUL,
		0x666f510fUL, 0xd66bce5bUL, 0x53128269UL, 0x2a5bc8deUL,
		0x33224cf4UL, 0x404f5f67UL, 0xb9c4ddb2UL, 0x3108f01eUL,
		0xe96ef837UL, 0x832dd1d8UL, 0x9ce4657fUL, 0x1fc750e2UL,
		0xe56ddc7aUL, 0x3ca08e2bUL, 0xe1681392UL, 0x5349c63bUL,
		0x15573eaeUL, 0x5bd785baUL, 0xe1fa83ebUL, 0x5860ef1aUL,
		0x97a1a781UL, 0xe7c030a1UL, 0x3bf7f3beUL, 0x48fb8c89UL,
		0x33396e99UL, 0x3f5b1c31UL, 0x01d54503UL, 0x32c2a394UL,
		0x64a3442cUL, 0xf0649355UL, 0xe2f14dacUL, 0xf00003d4UL,
		0xa8682317UL, 0x36d2d946UL, 0x7baba2a1UL, 0x1b8bbc58UL,
		0xdbdc7cd8UL, 0xb61282d3UL, 0xcf7d65bfUL, 0x31b35f75UL,
		0x40be8c50UL, 0xf946f67aUL, 0x450c18daUL, 0xc5795ef1UL,
		0x4ccbbf3dUL, 0x43ebf4f2UL, 0xc142ea50UL, 0x743c5aa2UL,
		0xca0a694aUL, 0x32955c4fUL, 0x976a1201UL, 0xfb44d6d3UL,
		0xe3883664UL, 0xed0a2672UL, 0x254f61d6UL, 0xd69a18a0UL,
		0x8aa18f2eUL, 0xee03607fUL, 0x1efbf353UL, 0xa214f8efUL,
		0x327d7d42UL, 0x3d9c5f31UL, 0x5199a7feUL, 0xb26102f0UL,
		0x7141e13bUL, 0xbcdb90d9UL, 0xab5d6e43UL, 0x9fe13e24UL,
		0xd52cd201UL, 0x27be8644UL, 0x5e74f8d4UL, 0x7afa882bUL,
		0xbde373faUL, 0x33c610c5UL, 0x247565b1UL, 0xf3e2d8bcUL,
		0xf68276fcUL, 0xba47267fUL, 0xf73c50a0UL, 0xedf36518UL,
		0x54be80d8UL, 0x1e853affUL, 0x4177cdd7UL, 0xfcc9260dUL,
		0xe6990f32UL, 0x17123409UL, 0x56492020UL, 0xcf1cfbd2UL,
		0x5d69f234UL, 0x10cee084UL, 0x8ed5b7fcUL, 0x041fc1e1UL,
		0xcd5f19acUL, 0xb1014231UL, 0x777c44d3UL, 0x9fa34027UL,
		0x8bda12ccUL, 0xcb0f8867UL, 0x7d2ccacdUL, 0x2f6d136bUL,
		0xb79c5a8cUL, 0xaa330759UL, 0x35b7bc90UL, 0x1f42046cUL,
		0x590e87b9UL, 0x2dc89592UL, 0x2819e1ffUL, 0xdd1e9ca1UL,
		0x2690ed1dUL, 0x05034434UL, 0xab4633b2UL, 0xc44fc6efUL,
	},
	{   1UL,
		0x00010001UL,
	}
};

/* Openssl command To generate private key: Openssl genrsa -out private.pem
The below key is in Bigint format please check in Api guide to know about Bigint format*/
static const struct AsymCrypt_RSAPrivkey gPkaRsaPrivateKey =
{
	{
		128UL,
		0x7b0e1331UL, 0x21d93fafUL, 0xf24ace7cUL, 0x2e1c2805UL,
		0x2505b8eaUL, 0x789f6d3bUL, 0x1429332dUL, 0x410ebcb3UL,
		0x11765fc0UL, 0xa37281bcUL, 0xdcb72cd4UL, 0x057f8582UL,
		0xfbc664dbUL, 0x1c68e56dUL, 0x483dfb84UL, 0x01d8ce5fUL,
		0x666f510fUL, 0xd66bce5bUL, 0x53128269UL, 0x2a5bc8deUL,
		0x33224cf4UL, 0x404f5f67UL, 0xb9c4ddb2UL, 0x3108f01eUL,
		0xe96ef837UL, 0x832dd1d8UL, 0x9ce4657fUL, 0x1fc750e2UL,
		0xe56ddc7aUL, 0x3ca08e2bUL, 0xe1681392UL, 0x5349c63bUL,
		0x15573eaeUL, 0x5bd785baUL, 0xe1fa83ebUL, 0x5860ef1aUL,
		0x97a1a781UL, 0xe7c030a1UL, 0x3bf7f3beUL, 0x48fb8c89UL,
		0x33396e99UL, 0x3f5b1c31UL, 0x01d54503UL, 0x32c2a394UL,
		0x64a3442cUL, 0xf0649355UL, 0xe2f14dacUL, 0xf00003d4UL,
		0xa8682317UL, 0x36d2d946UL, 0x7baba2a1UL, 0x1b8bbc58UL,
		0xdbdc7cd8UL, 0xb61282d3UL, 0xcf7d65bfUL, 0x31b35f75UL,
		0x40be8c50UL, 0xf946f67aUL, 0x450c18daUL, 0xc5795ef1UL,
		0x4ccbbf3dUL, 0x43ebf4f2UL, 0xc142ea50UL, 0x743c5aa2UL,
		0xca0a694aUL, 0x32955c4fUL, 0x976a1201UL, 0xfb44d6d3UL,
		0xe3883664UL, 0xed0a2672UL, 0x254f61d6UL, 0xd69a18a0UL,
		0x8aa18f2eUL, 0xee03607fUL, 0x1efbf353UL, 0xa214f8efUL,
		0x327d7d42UL, 0x3d9c5f31UL, 0x5199a7feUL, 0xb26102f0UL,
		0x7141e13bUL, 0xbcdb90d9UL, 0xab5d6e43UL, 0x9fe13e24UL,
		0xd52cd201UL, 0x27be8644UL, 0x5e74f8d4UL, 0x7afa882bUL,
		0xbde373faUL, 0x33c610c5UL, 0x247565b1UL, 0xf3e2d8bcUL,
		0xf68276fcUL, 0xba47267fUL, 0xf73c50a0UL, 0xedf36518UL,
		0x54be80d8UL, 0x1e853affUL, 0x4177cdd7UL, 0xfcc9260dUL,
		0xe6990f32UL, 0x17123409UL, 0x56492020UL, 0xcf1cfbd2UL,
		0x5d69f234UL, 0x10cee084UL, 0x8ed5b7fcUL, 0x041fc1e1UL,
		0xcd5f19acUL, 0xb1014231UL, 0x777c44d3UL, 0x9fa34027UL,
		0x8bda12ccUL, 0xcb0f8867UL, 0x7d2ccacdUL, 0x2f6d136bUL,
		0xb79c5a8cUL, 0xaa330759UL, 0x35b7bc90UL, 0x1f42046cUL,
		0x590e87b9UL, 0x2dc89592UL, 0x2819e1ffUL, 0xdd1e9ca1UL,
		0x2690ed1dUL, 0x05034434UL, 0xab4633b2UL, 0xc44fc6efUL,
	},
	{
		1UL,
		0x00010001UL,
	},
	{
		128UL,
		0x10419db5UL, 0x3627a236UL, 0xd5849feaUL, 0x3282d947UL,
		0xd2ea3de8UL, 0x1b1e8b4bUL, 0x902fda40UL, 0x4c67aa7aUL,
		0xd498779bUL, 0x7487d923UL, 0x7a7eef40UL, 0x3d074813UL,
		0x0e15d488UL, 0xfe0ad4d1UL, 0x1bd3fa06UL, 0xcd4add45UL,
		0x9fbe8cc8UL, 0xc0a9a662UL, 0xd548522dUL, 0xdf2922e1UL,
		0xfe3de915UL, 0xc348fc61UL, 0xfe2ff563UL, 0xb812d879UL,
		0x6336a88fUL, 0x7fa5a7d0UL, 0x4ff33e9fUL, 0x251eb5a3UL,
		0xaeff0201UL, 0x4bcb2073UL, 0x2282f9b4UL, 0xe0d83f5aUL,
		0x41e440b4UL, 0x219afde3UL, 0x28559641UL, 0xf4abdcdaUL,
		0xe3b49b85UL, 0xf178d643UL, 0xa408c55dUL, 0x0ee6d9a5UL,
		0xa6781eb4UL, 0x0e991ebaUL, 0xa13398bfUL, 0x449e1b56UL,
		0xcf15116cUL, 0x11302565UL, 0x4edb594fUL, 0xd749c93dUL,
		0x838eac8aUL, 0x2ad2d58dUL, 0x55eb8d39UL, 0xaa15bd0eUL,
		0xc6c0ec10UL, 0xf977ba52UL, 0xfb8c380aUL, 0xda9cab52UL,
		0x88bf4b3fUL, 0x51a28f3bUL, 0xd1b2fec1UL, 0x0b9924f4UL,
		0x96e773a6UL, 0x28b28238UL, 0xdcd83accUL, 0xca8ab55cUL,
		0xd2db3868UL, 0x2ddbed80UL, 0x14c55619UL, 0x06f388afUL,
		0xe48436c4UL, 0x211ecf5fUL, 0x5213bfecUL, 0x4b7df219UL,
		0x46f80707UL, 0x2ce17127UL, 0xd8ad3061UL, 0x3da46129UL,
		0xd117f13dUL, 0x748feefcUL, 0x5c757effUL, 0xe857d95cUL,
		0xb4542a7bUL, 0x1498c4e4UL, 0xfa001cf0UL, 0xcf00c0beUL,
		0x56efbfcdUL, 0x9e803291UL, 0xb55023cbUL, 0x266005faUL,
		0xcb4dc5f2UL, 0x1710b011UL, 0x91443865UL, 0x9dbf9f2fUL,
		0x3b9cf301UL, 0x27716c63UL, 0x3a733ac0UL, 0xd8a9d375UL,
		0xd3b836a2UL, 0x597c61ffUL, 0xc72cd571UL, 0x29e252c5UL,
		0x17c4800dUL, 0x9a552400UL, 0xca817f97UL, 0xac48ac07UL,
		0xeb161001UL, 0x22489aa6UL, 0xe66e63f6UL, 0x6101a293UL,
		0xa866553dUL, 0x88e654bfUL, 0xcd49f414UL, 0xa1a179e0UL,
		0x209c323dUL, 0x0d67d0c1UL, 0x6e49cb85UL, 0x607f314aUL,
		0xe0171a47UL, 0xf45bc7cfUL, 0xff4699c2UL, 0xa53a5faeUL,
		0x43a9d8b6UL, 0x80311a85UL, 0xefd571d8UL, 0x1a2fc991UL,
		0xef87566bUL, 0x86524b55UL, 0xa81e866eUL, 0x86701fcbUL,
	},
	{
		64UL,
		0xbb427bfbUL, 0x20751184UL, 0xa3c30577UL, 0x57860858UL,
		0x91b400daUL, 0x039abd73UL, 0xde2ab0e0UL, 0x9f7461a7UL,
		0xb0a8eefaUL, 0xd243edeeUL, 0xfe98727eUL, 0x0e16b7a8UL,
		0x3ee2a197UL, 0xa4818a70UL, 0x0252031aUL, 0x0d319871UL,
		0x74000a85UL, 0x92322a99UL, 0x97528ccbUL, 0x379706b8UL,
		0xdec994aeUL, 0xa2b1c043UL, 0x9304d233UL, 0x1bb04a5aUL,
		0x7d5011f3UL, 0xc196fc5cUL, 0xa77e67c6UL, 0x93391a97UL,
		0x332579dbUL, 0x8fae025fUL, 0x916b27daUL, 0x9f68ff9cUL,
		0xd38bdc5bUL, 0x7f51ded1UL, 0x9e4b925cUL, 0x54fd6e39UL,
		0xf6018e7eUL, 0x037a0515UL, 0x0f37b15fUL, 0xa0c8abbdUL,
		0x775c6d6dUL, 0xc26846adUL, 0x822fc2e2UL, 0x81d88215UL,
		0x9dd17935UL, 0x5691073bUL, 0x90177682UL, 0xf2a7ee34UL,
		0x3464cd12UL, 0x1dc7785bUL, 0x90d5648fUL, 0x4c6aac82UL,
		0xc95e0ac0UL, 0x88aedb5fUL, 0x878e6626UL, 0xf792a736UL,
		0xd90fb07aUL, 0xc5b9583dUL, 0xa90c3640UL, 0xf21ba87aUL,
		0xcdb2dcf5UL, 0xe3a516b1UL, 0xbae16d87UL, 0xe28e0fc2UL,
	},
	{
		64UL,
		0xf77079c3UL, 0x49377587UL, 0x1e129263UL, 0x3c3f8120UL,
		0xb5a82126UL, 0x674d602dUL, 0x0e3c3073UL, 0x223eae9bUL,
		0xca12c56aUL, 0x8b9a6839UL, 0x7fb49cd3UL, 0xa8ee610fUL,
		0x696f7ec2UL, 0x282b3d3dUL, 0x4e387622UL, 0x5fff85a6UL,
		0x0d224c23UL, 0x35676878UL, 0x444eca3dUL, 0x62984d27UL,
		0xdc601db2UL, 0x4e7c65dbUL, 0x041c37c2UL, 0x5baf3823UL,
		0x228e69d6UL, 0xda9c94b6UL, 0xff406ed0UL, 0x32ee08d4UL,
		0x07edfe8fUL, 0x3c8f45d3UL, 0x9036cdafUL, 0x23eba296UL,
		0x09aa1a8bUL, 0x588982c1UL, 0xce249fe5UL, 0xccd0697fUL,
		0x2123fbd0UL, 0x872e4a00UL, 0xf8dd8629UL, 0xd3339685UL,
		0x0a8e7e7dUL, 0xc9c1db9dUL, 0x887b0dceUL, 0xab03b817UL,
		0x12a618f6UL, 0xf01a7a81UL, 0x804a6e25UL, 0x7c1f6999UL,
		0xecffb98eUL, 0x972a3c24UL, 0x71363317UL, 0x4ff950d6UL,
		0xf20754a5UL, 0xcd2bb01bUL, 0xdd44a55aUL, 0x547b9e5bUL,
		0x00be92bcUL, 0x1ab22b80UL, 0xd5ffa49cUL, 0xc52f5616UL,
		0xaa3f5d8eUL, 0x37dacba3UL, 0xdd2285f8UL, 0xddd375c9UL,
	},
	{
		64UL,
		0x385fa1d3UL, 0x25079e5dUL, 0x62c75315UL, 0x7c22bdafUL,
		0xf38f2b75UL, 0x33eabe6cUL, 0x7f4a901cUL, 0x232641d8UL,
		0x81dfbaffUL, 0x0dd63792UL, 0xc4d0aca6UL, 0xbe950e8bUL,
		0x635be3a3UL, 0x8f205599UL, 0xefb6225cUL, 0xc7eb6f0bUL,
		0x6ba2c01aUL, 0x3435bf4cUL, 0x2747711bUL, 0x41e4d8e8UL,
		0x4632b238UL, 0x39b36a11UL, 0xe4950e89UL, 0xe35ba249UL,
		0x28fa2a4aUL, 0xc48770a6UL, 0xc849e4d7UL, 0x2b773fceUL,
		0xc0af596bUL, 0xf67f0575UL, 0x7fc7056aUL, 0xadf756e4UL,
		0xe73fa862UL, 0x3e0b2254UL, 0x25e0db0fUL, 0xc7b6354dUL,
		0xc09cc298UL, 0xa6093906UL, 0xdd749008UL, 0x699e5d66UL,
		0x63a7d948UL, 0x86c4ad04UL, 0x9217cc60UL, 0xafdb7b53UL,
		0x862ca3efUL, 0x119de261UL, 0xe03fb34eUL, 0x374642a9UL,
		0xdc6932cfUL, 0x76a6d732UL, 0x28292b35UL, 0xb3ce65a2UL,
		0xcae36c2bUL, 0xd1d63ec9UL, 0x3b359689UL, 0x13ec8f26UL,
		0x09be568fUL, 0x81017021UL, 0x9b8cc5baUL, 0x969ead27UL,
		0xc7b84c86UL, 0xe5b509eeUL, 0xc07eafd5UL, 0xa6b1fa7cUL,
	},
	{
		64UL,
		0x81028f61UL, 0xb21c2fc0UL, 0x45eba179UL, 0x350e30b9UL,
		0xd96ea46aUL, 0x301f730bUL, 0x40f3af1dUL, 0xe0c6307dUL,
		0xf6954d0dUL, 0x801d8571UL, 0x7155d536UL, 0x9cafd2a8UL,
		0x2548c02fUL, 0xa672c802UL, 0x7eb96128UL, 0xbf96fcb1UL,
		0xd6e9149aUL, 0x90c61adbUL, 0x2bd62fb8UL, 0x2b6b7fc8UL,
		0xf2e97f86UL, 0x4d012c9eUL, 0x504d0dcfUL, 0xf9589109UL,
		0x57adda85UL, 0x52fc3692UL, 0x1452e502UL, 0xcfd04eb3UL,
		0x772411afUL, 0xc6a5c54fUL, 0x2ec8262eUL, 0xf4638629UL,
		0x6782ee0dUL, 0xcc07cf86UL, 0x71d67368UL, 0x1ed27a91UL,
		0x98e60189UL, 0x15235971UL, 0x145306deUL, 0x6b8531daUL,
		0xe552d84fUL, 0xe5c79574UL, 0x1db17d82UL, 0x364b68a8UL,
		0x7e75cf7bUL, 0xd620d18cUL, 0x68a3c932UL, 0xe634bc8dUL,
		0xa2e5261dUL, 0x23230d06UL, 0xb40ffb57UL, 0x5134fbb8UL,
		0x812aed7eUL, 0x27aef659UL, 0x648589bfUL, 0x17b41381UL,
		0xf588844eUL, 0x099272e0UL, 0xd0f71c74UL, 0xb05bf299UL,
		0xadb14466UL, 0x70fc7798UL, 0xab18606dUL, 0x7ac57e7bUL,
	},
	{
		64UL,
		0x2a234108UL, 0x26f10c77UL, 0x37e0a93eUL, 0xc94e27d0UL,
		0x620ab868UL, 0xc4866d92UL, 0x9501fbbbUL, 0x2d06af39UL,
		0xce96fc6fUL, 0x5ccdebb3UL, 0xea246fcbUL, 0xeddb7827UL,
		0x02ef25c5UL, 0x67599648UL, 0x27504ae2UL, 0x44d46183UL,
		0xa3f56968UL, 0x8d65fccaUL, 0x1bd53f81UL, 0x4d27cab5UL,
		0xaa24c5d1UL, 0x7270acc7UL, 0xa13ec3fdUL, 0x99180cc9UL,
		0xf18088ebUL, 0xdc972949UL, 0x59735d92UL, 0x0c860739UL,
		0x9fb2a99cUL, 0x0f25e5f2UL, 0x9bc17e3dUL, 0x97f5e8c0UL,
		0x63e7ee46UL, 0x0962df6bUL, 0x2c976df5UL, 0x88c0e9a9UL,
		0xb59730c0UL, 0x7efa7686UL, 0xebffbf5bUL, 0xaa4baeecUL,
		0xd5cec7b9UL, 0x4643bc53UL, 0xf736f7e8UL, 0x9ff978f6UL,
		0xdc563955UL, 0x04e95707UL, 0xb3ec6c74UL, 0x63db5cbaUL,
		0x12523994UL, 0x37986104UL, 0x09ed6e97UL, 0x1d635a00UL,
		0x3854fa3dUL, 0xfb3ebfecUL, 0x79c6d046UL, 0xa06c5cecUL,
		0xf6f3b75bUL, 0x17c3eb0aUL, 0xc92d3dd2UL, 0x18afbaebUL,
		0x1bbcc397UL, 0x4526cb3fUL, 0x76c2993fUL, 0x22bca33eUL,
	}
};

static const uint32_t gPkaRsaMessage[] =
{
	128UL,
	0x3c2bdf16UL, 0x4426b089UL, 0xc19b5111UL, 0xebf5e82aUL,
	0xeb87bff6UL, 0x639b4588UL, 0xa7514915UL, 0x83ff77d8UL,
	0x007556a2UL, 0x65061985UL, 0xc1b2252bUL, 0x96370d40UL,
	0x1a6a7c8aUL, 0x53810104UL, 0x3b7137acUL, 0xae8ff417UL,
	0xe745c58fUL, 0x3d78b4a6UL, 0x679258d6UL, 0x733d183eUL,
	0x3a455d3fUL, 0x75ee7450UL, 0x4bdaea60UL, 0x0dcc2b2eUL,
	0x8d3be612UL, 0x233be05aUL, 0x9ecbd99eUL, 0xa4fe1ccaUL,
	0x26c176cbUL, 0x28726e29UL, 0x44e80046UL, 0x2c475bc6UL,
	0xdf1e3b08UL, 0x53c3db73UL, 0x785cb9d5UL, 0xaa8db5d6UL,
	0xb3258545UL, 0x30a5a698UL, 0xf1d908edUL, 0x96bd44cdUL,
	0x2b8cfda5UL, 0x5f874cc7UL, 0xa3564a3dUL, 0xf84aa301UL,
	0xc46a6890UL, 0x9fb6a407UL, 0x970c4896UL, 0x8b000586UL,
	0x37c97c10UL, 0x9353b623UL, 0x55d07151UL, 0xdc965b03UL,
	0xf90afc05UL, 0x9487cc6aUL, 0x26426ac0UL, 0xa6f25dd5UL,
	0xb9247e1dUL, 0x8ed1204cUL, 0x01498a17UL, 0xa65219d2UL,
	0xfc9048d6UL, 0x2bb83efaUL, 0xbc325407UL, 0x83ca4732UL,
	0x066da945UL, 0xd1a58cb5UL, 0xbb2cad7dUL, 0xc6fdad15UL,
	0xce5fef80UL, 0x03bfc867UL, 0x8147ab51UL, 0x0af6313cUL,
	0x22eb6c73UL, 0x6950e9fcUL, 0x046c1bcbUL, 0x9b81b856UL,
	0xb64cc0e2UL, 0x5b245797UL, 0x3e2fd20cUL, 0xcdc12a2fUL,
	0x179cf3c2UL, 0xb38554aeUL, 0xfc187000UL, 0x0df189c7UL,
	0x3c74ce1aUL, 0xe30e0cf6UL, 0x2299e7cfUL, 0xb6f7d36cUL,
	0x383d1721UL, 0x2de9a694UL, 0x84f6c08cUL, 0xe7514144UL,
	0xf14b3243UL, 0xc03b9a2bUL, 0x1ece1045UL, 0xb7d26375UL,
	0x06c23d6bUL, 0x4a398ac9UL, 0xd277774cUL, 0xd3ae2286UL,
	0x4a99f3eaUL, 0xd12e6b9eUL, 0x0eafd882UL, 0x666d017cUL,
	0x628ade73UL, 0xc831d4d1UL, 0x94b6414aUL, 0x1f3d7d91UL,
	0x630073f1UL, 0xa560dd74UL, 0x47d3ce78UL, 0x1582a2e3UL,
	0x002cead0UL, 0xa00e6fa8UL, 0x971f3211UL, 0x95374ecdUL,
	0xf6f90e1fUL, 0xb67c3029UL, 0x445d5590UL, 0xc0ea1286UL,
	0xfaed59f4UL, 0x5f5db050UL, 0x7cd689f4UL, 0x464d5e77UL,
	0x04373960UL, 0x7d7026f1UL, 0x1f083072UL, 0x2790e411UL,
};

/* PKA handle for processing every api */
AsymCrypt_Handle			gPkaHandle = NULL;

void rsa_encryption_decryption(void *args)
{
    Drivers_open();
    Board_driversOpen();

	AsymCrypt_Return_t        status = ASYM_CRYPT_RETURN_SUCCESS;

    DebugP_log("[AsymCrypt] RSA Encryption and Decryption example started ...\r\n");

	/* Open PKA instance, enable PKA engine, Initialize clocks and Load PKA Fw */
    gPkaHandle = AsymCrypt_open(SA2UL_PKA_INSTANCE);
    DebugP_assert(gPkaHandle != NULL);

	/* Openssl Command for Encryption: Openssl rsautl -encrypt -raw -in message.bin -pubin -inkey public.pem -out message_encrypt.bin */
    status = AsymCrypt_RSAPublic(gPkaHandle, gPkaRsaMessage, &gPkaRsaPublicKey, gPkaRsaOutputResult);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	/* Openssl Command for Decryption: Openssl rsautl -decrypt -raw -in message_encrypt.bin -inkey private.pem -out message_decrypt.bin */
	status = AsymCrypt_RSAPrivate(gPkaHandle, gPkaRsaOutputResult, &gPkaRsaPrivateKey, gPkaRsaOutputResult);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	/* Close PKA instance, disable PKA engine, deinitialize clocks*/
	status = AsymCrypt_close(gPkaHandle);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	if (0 != memcmp(gPkaRsaOutputResult, gPkaRsaMessage, sizeof(gPkaRsaMessage)))
	{
		DebugP_log("[AsymCrypt] Decryption output did not match expected output\n");
	}

    DebugP_log("[AsymCrypt] Encryption and Decryption example completed!!\r\n");
    DebugP_log("All tests have passed!!\r\n");

    Board_driversClose();
    Drivers_close();

    return;
}