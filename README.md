## How to reproduce the demonstration

This repository demonstrates the difference between:

- `Bridge.provide()` (unsafe)
- `Bridge.provide_safe()` (thread-safe)

### Step 1 — Run the unsafe version

In `sketch.ino`, use:

Bridge.provide("update", updateCounter);

Upload and run the program.

After some time you should observe messages like:

> ERROR inconsistent state: counter=21 double=40
> ERROR inconsistent state: counter=RPC update: counter=1919 double= double=3836
> ERROR inconsistent state: counter=27 double=RPC update: counter=5227 double=54

This shows a race condition: `loop()` has read the variables while the RPC callback was updating them.

### Step 2 — Run the safe version

Now replace the line with:

Bridge.provide_safe("update", updateCounter);

Upload and run the program again.

The inconsistent state disappears.

---

### Conclusion

`Bridge.provide_safe()` prevents the race condition because the callback is executed in the same execution context as `loop()`.


# Bridge.provide_safe() vs Bridge.provide()

This repository demonstrates the practical difference between:

- `Bridge.provide()`
- `Bridge.provide_safe()`

in the Arduino UNO-Q RouterBridge architecture.

## Idea of the demonstration

The RPC callback updates two related variables:

- `counter`
- `doubleValue`

The program enforces the invariant:

`doubleValue = 2 × counter`

If the program is correct, this relationship must **always be true**.

## Behavior with Bridge.provide()

When using:

`Bridge.provide("update", updateCounter);`

the RPC callback runs in a **separate thread**.

Therefore `loop()` may read the Programming the UNO Q in C **while they are being updated**.

This may produce inconsistent states such as:

>counter = 19  
>doubleValue = 36  

which violates the rule:

`doubleValue = 2 × counter`

---

With `Bridge.provide()`, the RPC callback runs in a separate thread.

In this demonstrator, both the callback and `loop()` write to `Monitor`.
As a result, the serial output may become interleaved.

So, instead of a clean message such as:

> ERROR inconsistent state: counter=19 double=36

The actual result can sometimes look like this:

ERROR inconsistent state: counter=RPC update: counter=1919 double= double=3836

This happens because two execution contexts are writing to `Monitor` at the same time.

If we separate the two interleaved messages, we can understand them as:

> ERROR inconsistent state: counter=19 double=36
> RPC update: counter=19 double=38

This shows two things:
1. shared data may be observed in an inconsistent state
2. `Monitor` output itself may also become corrupted by concurrent access

This demonstrates a **race condition**.

## Behavior with Bridge.provide_safe()

When replacing the line with:

Bridge.provide_safe("update", updateCounter);

the callback is executed **in the same execution context as `loop()`**.

Execution becomes serialized:

```
loop()
callback
loop()
```

Therefore `loop()` can never read the variables during the update.

The inconsistent state disappears.

## How to reproduce

1. Run the program with:

Bridge.provide("update", updateCounter);

You will eventually observe:

ERROR inconsistent state

2. Replace it with:

Bridge.provide_safe("update", updateCounter);

Run again.

No inconsistent state will appear.

## Conclusion

`Bridge.provide_safe()` prevents `loop()` from observing partially updated shared data by ensuring that the RPC callback runs in the same execution context as `loop()`.

---

## Acknowledgments

The idea of this demonstration and the analysis of the behavior of  
`Bridge.provide()` versus `Bridge.provide_safe()` were developed with the help of ChatGPT (OpenAI).

The goal of this repository is to provide a minimal reproducible example that clearly demonstrates the race condition that can occur when using `Bridge.provide()` and how `Bridge.provide_safe()` prevents it.
