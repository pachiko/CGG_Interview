#include <vector>
#include <cmath>
#include <algorithm>

/// @brief Finds the index of the first occurence of an integer within the array. O(n)
/// @param arr: The array to search
/// @param x: The integer to search for
/// @return The index of the first occurence of x. -1 if x is not found in arr.
int findIndex(const std::vector<int>& arr, int x) {
	for (int i = 0; i < arr.size(); i++) if (arr[i] == x) return i;
	return -1;
}


/// @brief Computes the RMS-normalized array of the input array. O(n)
/// @param arr: The array to compute the RMS-normalized array
/// @return The RMS-normalized array. Empty if arr is invalid-sized.
std::vector<float> rmsNorm(const std::vector<float>& arr) {
	if (arr.size() < 1) return {};

	float rms = 0.f;
	for (auto& x : arr) rms += x * x;
	rms /= arr.size();
	rms = std::sqrtf(rms);

	std::vector<float> res(arr);
	for (auto& x : res) x /= rms;
	return res;
}


/// @brief Computes the linear convolution of two arrays of floats with equal lengths n. O(n^2)
/// @param a: The first array.
/// @param b: The second array.
/// @return The convolution result as an array, sized (n + n - 1)
std::vector<float> linConv(const std::vector<float>& a, const std::vector<float>& b) {
	if (a.size() < 1 || b.size() < 1) return {};

	int n = 2 * a.size() - 1;
	std::vector<float> c;
	c.reserve(n);

	// The indices for the overlap between the 2 arrays
	int start = 0, stop = 0;

	for (int i = 0; i < n; i++) {
		float sum = 0.f;

		// Iterate over arrays overlap
		int a_it = start;
		int b_it = stop;
				
		while (a_it <= stop && b_it >= start) {
			sum += a[a_it++] * b[b_it--];
		}

		// Change overlap
		if (i < a.size() - 1) stop++;
		else start++;

		c.push_back(sum);
	}

	return c;
}


/// @brief Computes the convolution of two arrays of floats with equal lengths n.
/// @param a: The first array.
/// @param b: The second array.
/// @return The convolution result as an array, sized (n + n - 1)
std::vector<float> convolution(const std::vector<float>& a, const std::vector<float>& b) {
	return linConv(a, b);
	// TODO: Implement circular convolution (frequency domain) instead of linear convolution (time/space) for large n.
	// Convolution in time-space is multiplication in frequency domain and vice-versa
	// 1) Zero-pad the input vectors to length 2*n - 1 {O(n)}
	// 2) Compute FFT of each {O(nlogn)}
	// 3) Element-wise multiplication {O(n)}
	// 4) Inverse-FFT the product {O(nlogn)}
	// Source: https://www.mathworks.com/help/signal/ug/linear-and-circular-convolution.html
	// Note that Cooley-Tukey FFT only works for powers of 2, so vectors should be padded to the nearest power of 2
	// Alternatively, use Bluestein's algorithm.
}

/// @brief Computes the mean-smoothed array of floats. O(n)
/// @param arr: The array of floats.
/// @param win: The window of size 2*w + 1. Must be odd.
/// @return The mean-smoothed array of floats.
std::vector<float> meanSmooth(const std::vector<float>& arr, uint32_t win) {
	int n = arr.size();

	// invalid if window is too small, window is even or array is smaller than window
	if (win % 2 == 0 || n < win) {
		return {};
	}
	else if (win == 1) { // trivial if window size is 1
		return arr;
	}

	std::vector<float> res;
	float sum = 0.f;
	
	// Indices for window extent
	int start = 0, stop = (win - 1)/2;

	// Pre-calculate sum for the first element
	for (int i = start; i <= stop; i++) sum += arr[i];

	for (int i = 0; i < n; i++) {
		res.push_back(sum/(stop - start + 1));

		if (stop < n - 1) { // window can expand right
			sum += arr[++stop];
			if ((stop - start + 1) > win) { // window shrinks left if oversized
				sum -= arr[start++];
			}
		}
		else { // window shrinks left
			sum -= arr[start++];
		}
	}

	return res;
}
 
int main() {
	// Test Search
	std::vector<int> s = { 2, 1, 1, 3 };
	auto idx = findIndex(s, 1); // 1
	idx = findIndex(s, 99); // -1
	idx = findIndex({}, 3); // -1
	
	// Test RMS
	auto rms = rmsNorm({}); // []
	std::vector<float> arr = { 1, 2, 3, 4 };
	rms = rmsNorm(arr); // [0.36514837 0.73029674 1.09544512 1.46059349]
	
	// Test Convolution
	auto c = convolution({}, {}); // []
	std::vector<float> a = { 1, 2, 3 };
	std::vector<float> b = { 4, 5, 6 };
	c = convolution(a, {}); // []
	c = convolution({}, b); // []
	c = convolution(a, b); // [4 13 28 27 18]
	c = convolution(b, a); // [4 13 28 27 18]

	a = { 1 };
	b = { 2 };
	c = convolution(a, b); // [2]

	a = { 1, 2, 3, 4 };
	b = { 5, 6, 7, 8 };
	c = convolution(a, b); // [5 16 34 60 61 52 32]

	// Test Smoothing
	arr = { 1, 2, 3, 4, 5 };
	auto smooth = meanSmooth(arr, 3); // [1.5, 2, 3, 4, 4.5]
	smooth = meanSmooth({ 1 }, 3); // []
	smooth = meanSmooth({ 1 }, 1); // [1]
	smooth = meanSmooth(arr, -1); // []
	smooth = meanSmooth(arr, 2); // []
	smooth = meanSmooth(arr, 1); // [1 2 3 4 5]
	smooth = meanSmooth(arr, 5); // [2, 2.5, 3, 3.5, 4]
	smooth = meanSmooth(std::vector<float>(8, 2), 3); // all 2's

	return 0;
}