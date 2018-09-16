////////////////////////////////////////////////////////////
//
// The MIT License(MIT)
// 
// Copyright(c) 2015, 2016 Ben Deane
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////////

// This code is enterely based on Ben Deane's (AKA elbeno) work. Thank you so much.
// Github Repository: https://github.com/elbeno/constexpr

#pragma once

namespace DonerComponents
{
	namespace cx
	{
		struct str
		{
			const char* s;
			int len;
		};

		constexpr str stradd(const str& a, const str& b)
		{
			return { b.s, a.len + b.len };
		}

		constexpr str strlen(const str p, int maxdepth)
		{
			return *p.s == 0 || maxdepth == 0 ? p :
				strlen({ p.s + 1, p.len + 1 }, maxdepth - 1);
		}
		constexpr str strlen_bychunk(const str p, int maxdepth)
		{
			return *p.s == 0 ? p :
				strlen_bychunk(stradd({ 0, p.len }, strlen({ p.s, 0 }, maxdepth)), maxdepth);
		}

		// max recursion = 256 (strlen, especially of a long string, often happens at
		// the beginning of an algorithm, so that should be fine)
		constexpr int strlen(const char* s)
		{
			return strlen_bychunk(strlen({ s, 0 }, 256), 256).len;
		}

		constexpr unsigned murmur3_32_k(unsigned k)
		{
			return (((k * 0xcc9e2d51) << 15)
					| ((k * 0xcc9e2d51) >> 17)) * 0x1b873593;
		}

		constexpr unsigned murmur3_32_hashround(
			unsigned k, unsigned hash)
		{
			return (((hash^k) << 13)
					| ((hash^k) >> 19)) * 5 + 0xe6546b64;
		}

		constexpr unsigned word32le(const char* s, int len)
		{
			return
				(len > 0 ? static_cast<unsigned>(s[0]) : 0)
				+ (len > 1 ? (static_cast<unsigned>(s[1]) << 8) : 0)
				+ (len > 2 ? (static_cast<unsigned>(s[2]) << 16) : 0)
				+ (len > 3 ? (static_cast<unsigned>(s[3]) << 24) : 0);
		}

		constexpr unsigned word32le(const char* s)
		{
			return word32le(s, 4);
		}

		constexpr unsigned murmur3_32_loop(
			const char* key, int len, unsigned hash)
		{
			return len == 0 ? hash :
				murmur3_32_loop(
					key + 4,
					len - 1,
					murmur3_32_hashround(
						murmur3_32_k(word32le(key)), hash));
		}

		constexpr unsigned murmur3_32_end0(unsigned k)
		{
			return (((k * 0xcc9e2d51) << 15)
					| ((k * 0xcc9e2d51) >> 17)) * 0x1b873593;
		}

		constexpr unsigned murmur3_32_end1(unsigned k,
										   const char* key)
		{
			return murmur3_32_end0(
				k ^ static_cast<unsigned>(key[0]));
		}

		constexpr unsigned murmur3_32_end2(unsigned k,
										   const char* key)
		{
			return murmur3_32_end1(
				k ^ (static_cast<unsigned>(key[1]) << 8), key);
		}

		constexpr unsigned murmur3_32_end3(unsigned k,
										   const char* key)
		{
			return murmur3_32_end2(
				k ^ (static_cast<unsigned>(key[2]) << 16), key);
		}

		constexpr unsigned murmur3_32_end(unsigned hash,
										  const char* key, int rem)
		{
			return rem == 0 ? hash :
				hash ^ (rem == 3 ? murmur3_32_end3(0, key) :
						rem == 2 ? murmur3_32_end2(0, key) :
						murmur3_32_end1(0, key));
		}

		constexpr unsigned murmur3_32_final1(unsigned hash)
		{
			return (hash ^ (hash >> 16)) * 0x85ebca6b;
		}
		constexpr unsigned murmur3_32_final2(unsigned hash)
		{
			return (hash ^ (hash >> 13)) * 0xc2b2ae35;
		}
		constexpr unsigned murmur3_32_final3(unsigned hash)
		{
			return (hash ^ (hash >> 16));
		}

		constexpr unsigned murmur3_32_final(unsigned hash, int len)
		{
			return
				murmur3_32_final3(
					murmur3_32_final2(
						murmur3_32_final1(hash ^ static_cast<unsigned>(len))));
		}

		constexpr unsigned murmur3_32_value(const char* key, int len,
											unsigned seed)
		{
			return murmur3_32_final(
				murmur3_32_end(
					murmur3_32_loop(key, len / 4, seed),
					key + (len / 4) * 4, len & 3),
				len);
		}

		constexpr unsigned murmur3_32(const char *key, unsigned seed = 0)
		{
			return murmur3_32_value(key, strlen(key), seed);
		}
	}
}
