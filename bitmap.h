#ifndef BITMAP_H
#define BITMAP_H

/*
 * A bitmap implementation based on std::vector<uint32_t>.
 * Supported operations:
 *   + get/set/reset/flip a specific bit.
 *   + count the number of 1's in the set.
 *   + size/resize to get/set its size.
 *   + clear/flip_all to change all the bits.
 *   + union_with/intersect_with/diff_with another Bitmap
 */

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace compiler_skeleton::utils
{

class Bitmap
{
  protected:
	std::vector<uint32_t> _bits;
	size_t _size;

	inline static size_t _ceil_div32(size_t x) { return (x + 31) >> 5; }
	inline static size_t _div32(size_t x) { return x >> 5; }
	inline static int _remain_div32(size_t x) { return static_cast<int>(x & 31); }

  public:
	Bitmap() = default;
	Bitmap(int size)
	  : _bits(_ceil_div32(size), 0), _size(size) {}

	inline size_t size() const { return _size; }
	inline void resize(size_t size) // Note: we do not set the newly added bits to 0.
		{ _bits.resize(_ceil_div32(size)); _size = size; }

	// getters & setters
	inline bool get(size_t idx) const
		{ return (_bits.at(_div32(idx)) >> _remain_div32(idx)) & 1; }
	inline void set(size_t idx)
		{ _bits.at(_div32(idx)) |= 1 << _remain_div32(idx); }
	inline void reset(size_t idx)
		{ _bits.at(_div32(idx)) &= ~(1 << _remain_div32(idx)); }
	inline void set(size_t idx, bool val)
		{ val? set(idx): reset(idx); }
	inline void flip(size_t idx)
		{ _bits.at(_div32(idx)) ^= 1 << _remain_div32(idx); }
	
	// unary operations
	void clear();
	size_t cnt() const;
	void flip_all();

	// binary operations
	void union_with(const Bitmap &other);
	void intersect_with(const Bitmap &other);
	void diff_with(const Bitmap &other);
};

} // compiler_skeleton::utils

#endif