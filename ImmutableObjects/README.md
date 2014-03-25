
# Immutable Objects

The purpose of this experiment is to find the best way of implementing classes for use in a concurrent (multi-threaded) environment.

The objectives are:

1. Clean, simple code
2. Avoid most concurrency issues by using immutable objects
3. Protect mutable objects where changes take place
4. Efficient handling of values with move semantics

## Sample Project Requirements

To explore this, we are going to create a simple `Person` class that holds an individuals given, family and alias names.

In the western world, most people have one or more given (first) names, and one (sometimes more) family names.  Double barrelled names are considered to be a single family name, but some cultures have multiple family names, although the individual may be commonly known by only one given name and one family name (for example in my case, David Meehan, although I have a second given name, John.  I am commonly known by an abbreviation of my first name, Dave.

This could be most simply represented by the following struct:

```cpp

struct Person {

    std::vector<std::string> givenNames;
    std::vector<std::string> familyNames;
    std::vector<std::string> aliases;
    
};
```

We need to provide some validation that the member values cannot have invalid data provided.

* givenNames - should be zero or more non-empty strings
* familyNames - should be zero or more non-empty strings
* aliases - should be zero or more non-empty strings

Values assigned from a UI might well contain erroneous values, such as leading or trailing spaces, or zero length strings.  The Person class must prevent instances from being created with invalid values.  It can do so by throwing meaningful exceptions when invalid assignments are made.

All objects must support std::ostream insertion.

All objects must be immutable by default.  This can be provided by use of the `const` keyword, or by providing a class with only `const` methods and no public member data.  Helper classes can be provided to facilitate mutabilility.

## Benefits of Immutable Objects

One of the best ways of avoiding concurrency problems is to avoid mutating an object in one thread whilst it is being consumed by another.  Even worse is likely to be mutating the same instance in two or more threads at the same time, the outcome is most likely to be undesireable.

Once immutable objects are employed, the developer needs only to consider coordinating when objects are mutated and provide appropriate protections.

If consumers of objects work on a snapshot of the object when they first acquire access, it doesn't matter if another thread mutates the original object concurrently.

Let's expand on our implementation to provide an immutable class of type Person:



