# Understanding Floating-Point Summation: A Practical Guide

*DISCLAIMER: this text has been produce by Gemini 3.0 Pro, with the prompt «Consider this code (the `kahan_summation.c`), which shows the non-commutative nature of summation in IEEE floating point arithmetic testing many different summation orders, including the kahan summation. Get through the code and write a commentary / guide for the student, in a markdown file»*



Welcome to one of the most notorious "gotchas" in High Performance Computing. 

When you first learned arithmetic, you were taught that addition is commutative ($a+b = b+a$) and associative ($(a+b)+c = a+(b+c)$). In the digital world of IEEE 754 floating-point numbers, **associativity is strictly false**. 

This provided code, `kahan_summation.c`, is designed to prove this to you experimentally. By changing the *order* in which an array of numbers is summed, or by changing the method of accumulation, you will get different final results.

Here is a breakdown of what the code is doing, why it happens, and how we fix it.

---

## 1. The Root of the Problem: "Swamping"
A `float` (single precision) only has 24 bits of mantissa (about 7 decimal digits of precision). If your accumulator `SUM` grows very large, and you try to add a very small `float` to it, the hardware must align their decimal points before adding. 

If the smaller number is shifted so far to the right that its significant digits fall off the edge of the 24-bit mantissa, those digits are completely lost to rounding. The addition effectively does nothing. This is known as **swamping**.

---

## 2. The Three Contenders

The code tests three different strategies for summing the exact same array of floating-point numbers:

### A. `normal_summation` (The Naive Approach)
```c
float SUM = 0;
for (i = 0; i < N; i++) SUM += array[i];

```

This is the standard, textbook loop. It uses a `float` accumulator to sum a `float` array. It is highly susceptible to swamping. As `SUM` grows, the later elements in the array are progressively truncated.

### B. `normal_dsummation` (The Brute-Force Fix)

```c
long double SUM = 0;
// ... loop ...
return (float)SUM;

```

Here, we accumulate the `float` values into a `long double` (typically 80-bit or 128-bit precision on x86 architectures). Because the accumulator has a massive mantissa, the small `float` values almost never fall off the edge. This represents the "true" or "reference" mathematical sum. However, promoting types like this can be slow and isn't always feasible in memory-constrained or heavily vectorized environments.

### C. `kahan_summation` (The Mathematical Elegance)

Invented by William Kahan (the primary architect of the IEEE 754 standard), this algorithm uses an extra variable `c` to "remember" the low-order bits that were lost during the addition.

```c
float c = 0, t, y;
for (i = 0; i < N; i++) {
    y = array[i] - c;   // Subtract the lost bits from the previous iteration
    t = SUM + y;        // Add to the accumulator (swamping happens here!)
    c = (t - SUM) - y;  // Recover the exact bits that were lost
    SUM = t;            // Store the new sum
}

```

* `t - SUM` yields the high-order bits of `y` that successfully made it into the accumulator.
* Subtracting `y` from that yields the *negative* of the bits that were truncated.
* In the next iteration, `-c` adds those lost bits back into the next array element!

---

## 3. The Experiments

The `main` function runs a series of tests to show exactly how much the naive sum deviates from the precise sums.

### Experiment 1: Unsorted Random Array

The array is filled with random numbers of varying magnitudes. You will notice that the `normal` summation immediately begins to drift from the `dnormal` (long double) reference. The Kahan summation, however, stays remarkably accurate using only `float` variables.

### Experiment 2: Sorted Array (Small to Large)

```c
qsort(array, N, sizeof(float), compare);

```

The code sorts the array and sums it from the smallest numbers to the largest. **This is the optimal way to do a naive summation.** By summing the tiny numbers together first, they build up into a larger subtotal before they are finally added to the massive numbers at the end. Because they are added to numbers of similar magnitude, swamping is minimized.

### Experiment 3: Reverse Sorted Array (Large to Small)

```c
normal_summation(array, N, 1); // The '1' flag triggers the reverse loop

```

This is the worst-case scenario. We start the sum with the largest numbers in the array. The accumulator `SUM` becomes huge on the very first few iterations. By the time the loop reaches the tiny numbers at the end of the array, they are completely swamped and ignored. The error here will be significantly higher than in Experiment 2.

### Experiment 4 & 5: The Harmonic Series

*(Note: The `printf` statements in the code label this as `1/n^2`, but the code `(float)((double)1.0/(double)i)` actually generates the harmonic series $1/n$.)*

The array is filled with the sequence $1, 1/2, 1/3, 1/4, \dots, 1/N$.
This is a classic test for floating-point accuracy because the terms get progressively smaller.

* Summing from $1/N$ up to $1$ (Small to Large) preserves precision.
* Summing from $1$ down to $1/N$ (Large to Small) destroys precision. The accumulator hits a point where adding $1/1000000$ does absolutely nothing to the sum. Kahan summation will elegantly capture every single fractional bit, matching the `long double` accumulator almost perfectly.

---

## Key Takeaways

1. **Never assume $(a+b)+c = a+(b+c)$ in your code.**
2. If you must use a standard loop to sum an array of vastly different magnitudes, **sort it from smallest to largest first**.
3. When absolute precision is required in large reductions (e.g., N-body simulations, fluid dynamics), use **Kahan Summation** or accumulate into a higher-precision data type.

```

**Would you like me to outline how modern compilers handle (or struggle with) vectorizing the Kahan summation loop, so you can tie this back to the `-fassociative-math` discussion from the previous slides?**

``Here is a comprehensive commentary and guide designed to accompany your `kahan_summation.c` code. It is formatted in Markdown and strikes the balance between scientific precision and an accessible, informal tone suitable for lecture notes or a GitHub `README.md`.

```markdown
# Understanding Floating-Point Summation: A Practical Guide

Welcome to one of the most notorious "gotchas" in High Performance Computing. 

When you first learned arithmetic, you were taught that addition is commutative ($a+b = b+a$) and associative ($(a+b)+c = a+(b+c)$). In the digital world of IEEE 754 floating-point numbers, **associativity is strictly false**. 

This provided code, `kahan_summation.c`, is designed to prove this to you experimentally. By changing the *order* in which an array of numbers is summed, or by changing the method of accumulation, you will get different final results.

Here is a breakdown of what the code is doing, why it happens, and how we fix it.

---

## 1. The Root of the Problem: "Swamping"
A `float` (single precision) only has 24 bits of mantissa (about 7 decimal digits of precision). If your accumulator `SUM` grows very large, and you try to add a very small `float` to it, the hardware must align their decimal points before adding. 

If the smaller number is shifted so far to the right that its significant digits fall off the edge of the 24-bit mantissa, those digits are completely lost to rounding. The addition effectively does nothing. This is known as **swamping**.

---

## 2. The Three Contenders

The code tests three different strategies for summing the exact same array of floating-point numbers:

### A. `normal_summation` (The Naive Approach)
```c
float SUM = 0;
for (i = 0; i < N; i++) SUM += array[i];

```

This is the standard, textbook loop. It uses a `float` accumulator to sum a `float` array. It is highly susceptible to swamping. As `SUM` grows, the later elements in the array are progressively truncated.

### B. `normal_dsummation` (The Brute-Force Fix)

```c
long double SUM = 0;
// ... loop ...
return (float)SUM;

```

Here, we accumulate the `float` values into a `long double` (typically 80-bit or 128-bit precision on x86 architectures). Because the accumulator has a massive mantissa, the small `float` values almost never fall off the edge. This represents the "true" or "reference" mathematical sum. However, promoting types like this can be slow and isn't always feasible in memory-constrained or heavily vectorized environments.

### C. `kahan_summation` (The Mathematical Elegance)

Invented by William Kahan (the primary architect of the IEEE 754 standard), this algorithm uses an extra variable `c` to "remember" the low-order bits that were lost during the addition.

```c
float c = 0, t, y;
for (i = 0; i < N; i++) {
    y = array[i] - c;   // Subtract the lost bits from the previous iteration
    t = SUM + y;        // Add to the accumulator (swamping happens here!)
    c = (t - SUM) - y;  // Recover the exact bits that were lost
    SUM = t;            // Store the new sum
}

```

* `t - SUM` yields the high-order bits of `y` that successfully made it into the accumulator.
* Subtracting `y` from that yields the *negative* of the bits that were truncated.
* In the next iteration, `-c` adds those lost bits back into the next array element!

---

## 3. The Experiments

The `main` function runs a series of tests to show exactly how much the naive sum deviates from the precise sums.

### Experiment 1: Unsorted Random Array

The array is filled with random numbers of varying magnitudes. You will notice that the `normal` summation immediately begins to drift from the `dnormal` (long double) reference. The Kahan summation, however, stays remarkably accurate using only `float` variables.

### Experiment 2: Sorted Array (Small to Large)

```c
qsort(array, N, sizeof(float), compare);

```

The code sorts the array and sums it from the smallest numbers to the largest. **This is the optimal way to do a naive summation.** By summing the tiny numbers together first, they build up into a larger subtotal before they are finally added to the massive numbers at the end. Because they are added to numbers of similar magnitude, swamping is minimized.

### Experiment 3: Reverse Sorted Array (Large to Small)

```c
normal_summation(array, N, 1); // The '1' flag triggers the reverse loop

```

This is the worst-case scenario. We start the sum with the largest numbers in the array. The accumulator `SUM` becomes huge on the very first few iterations. By the time the loop reaches the tiny numbers at the end of the array, they are completely swamped and ignored. The error here will be significantly higher than in Experiment 2.

### Experiment 4 & 5: The Harmonic Series

*(Note: The `printf` statements in the code label this as `1/n^2`, but the code `(float)((double)1.0/(double)i)` actually generates the harmonic series $1/n$.)*

The array is filled with the sequence $1, 1/2, 1/3, 1/4, \dots, 1/N$.
This is a classic test for floating-point accuracy because the terms get progressively smaller.

* Summing from $1/N$ up to $1$ (Small to Large) preserves precision.
* Summing from $1$ down to $1/N$ (Large to Small) destroys precision. The accumulator hits a point where adding $1/1000000$ does absolutely nothing to the sum. Kahan summation will elegantly capture every single fractional bit, matching the `long double` accumulator almost perfectly.

---

## Key Takeaways

1. **Never assume $(a+b)+c = a+(b+c)$ in your code.**
2. If you must use a standard loop to sum an array of vastly different magnitudes, **sort it from smallest to largest first**.
3. When absolute precision is required in large reductions (e.g., N-body simulations, fluid dynamics), use **Kahan Summation** or accumulate into a higher-precision data type.



---

---



Then, let’s add a further note, which nicely interacts with the note on trivial optimization and IEEE mah.

## 4. The Vectorization Dilemma: Kahan vs. `-fassociative-math`

This brings us directly into the discussion on compiler optimizations and the `-fassociative-math` flag. 

If you look closely at the Kahan algorithm, you will notice a severe **loop-carried dependency**:

```c
y = array[i] - c;   // Needs 'c' from the previous iteration
t = SUM + y;        // Needs 'SUM' from the previous iteration
c = (t - SUM) - y;  // Calculates new 'c' for the next iteration
SUM = t;
```

Because every iteration relies strictly on the c and SUM computed in the immediately preceding iteration, a standard compiler cannot automatically vectorize this loop using SIMD instructions (like AVX2 or AVX-512). It is forced to execute sequentially, which is significantly slower than a vectorized naive sum.

### Slow and steady wins the race
You might be tempted to think: "Aha! I will just use -ffast-math or -fassociative-math to force the compiler to parallelize it!"

Do this, and very likely it will completely destroy the algorithm.
In fact, remember that `-fassociative-math` grants the compiler permission to assume floating-point math behaves algebraically, exactly like at the blackboard: let's look at how an aggressive compiler optimizer algebraically simplifies the Kahan correction term `c`:

1. **Original code:** `c = (t - SUM) - y`
2. **Substitute `t`:** `c = ((SUM + y) - SUM) - y`
3. **Apply associativity:** `c = (SUM - SUM) + (y - y)`
4. **Simplify:** `c = 0.0`

If you compile the Kahan summation loop with `-fassociative-math` (or the broader `-ffast-math`), the compiler will statically determine that `c` is always exactly zero! It will silently optimize away your careful error-correction logic, collapsing the code back into the naive summation—completely defeating the purpose of writing the Kahan loop in the first place.

### The Takeaway for High Performance Computing

This perfectly illustrates the central tension in HPC: **Precision vs. Performance**.

- If you need maximum speed (Instruction Level Parallelism and Vectorization) and can tolerate minor precision loss, use a naive sum and enable `-fassociative-math`.
- If you need rigorous precision (e.g., aggregating global energy in a physics simulation to prevent energy drift) and must use Kahan summation, you **must** compile that specific function strictly adhering to IEEE 754 standards.ù

*(a SIMD-version of Kahan summation is possibile, though)*
