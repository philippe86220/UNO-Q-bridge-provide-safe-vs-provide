# Bridge.provide_safe() vs Bridge.provide()

This repository demonstrates the practical difference between:

- `Bridge.provide()`
- `Bridge.provide_safe()`

in the Arduino UNO-Q RouterBridge architecture.

## Idea of the demonstration

The RPC callback updates two related variables:

counter
doubleValue

The program enforces the invariant:

doubleValue = 2 × counter

If the program is correct, this relationship must **always be true**.

## Behavior with Bridge.provide()

When using:

Bridge.provide("update", updateCounter);

the RPC callback runs in a **separate thread**.

Therefore `loop()` may read the variables **while they are being updated**.

This may produce inconsistent states such as:

counter = 19  
doubleValue = 36  

which violates the rule:

doubleValue = 2 × counter

Example output:

ERROR inconsistent state: counter=19 double=36

This demonstrates a **race condition**.

## Behavior with Bridge.provide_safe()

When replacing the line with:

Bridge.provide_safe("update", updateCounter);

the callback is executed **in the same execution context as `loop()`**.

Execution becomes serialized:

loop()
callback
loop()

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
