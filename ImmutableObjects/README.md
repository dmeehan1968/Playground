
# Immutable Objects

The purpose of this experiment is to find the best way of implementing classes for use in a concurrent (multi-threaded) environment.

The objectives are:

1. Clean, simple code
2. Avoid most concurrency issues by using immutable objects
3. Protect mutable objects where changes take place
4. Efficient handling of values with move semantics

## Build Environment

This sample was built using Xcode 5 on Mac OS X Mavericks.  It uses C++11 code standards which should be compatible with other C++11 compliant compilers.

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

### A Basic, Immutable Class

Let's expand on our implementation to provide an immutable class of type Person:

Commit ad92f7e shows what our initial implementation is.  I've created type aliases in order to keep the code looking as clean as possible, whilst being descriptive to anyone using the API.

I've provided a stub implementation of an ostream inserter, though it doesn't output any of the member data at the moment (partly because we will need to provide handlers for the containers).

```cpp
Person dave({ "David", "John" }, { "Meehan" }, { "Dave" });
```

We've provided a constructor that takes initializer lists containing the names.  This is the most flexible way of handling a variety of inputs.  As you can see from the example usage in main.cpp, it copes with promotion of `const char *` to `std::string`.

If we use the debugger to print the value of the `dave` instance, this is what we get.

```text
(Person) $0 = {
  _givenNames = size=2 {
    [0] = "David"
    [1] = "John"
  }
  _familyNames = size=1 {
    [0] = "Meehan"
  }
  _aliases = size=1 {
    [0] = "Dave"
  }
}
```

We have not needed to declare the instance as `const` at this point, as the class itself is immutable.  The code will run fine if you do create a `const Person` with it.

## Does Immutability Guarentee Concurrency Protection?

As the class is by definition immutable, we have no prob;ems with concurrent access.  Or do we?

Let's think about a scenario.  If we are building an app to hold the details of more than one person, we are quite likely to use a standard container, such as vector, to store the instances.  Let's assume its defined as:

```cpp
std::vector<Person> people;
```

If we now pass a reference to a person from the container to another thread, perhaps to update a UI, and then delete the person from the collection, the reference will become invalid.  In fact, adding more people to the collection may make the references become invalid as the container allocates and possibly moves relocates storage as needed.

What we need is a way of ensuring that the Person instance exists independant of the thread in which a reference is available.

The obvious first solution is to use a Shared Pointer (such as std::shared_pointer) and store those in the container.

```
std::vector<std::shared_pointer<Person>> people;
```

Now when we add Person instances to the container, we are only storing a pointer.  When we give out copies of the shared pointer, its reference count is incremented to retain the Person instance whilst that reference exists.  Note that we always give out copies of Shared Pointers, not references to them.

This is a reasonable solution, but one that is a little error prone.  Person instances could still be allocated on the stack and passed to a thread for processing, with no guarentee that the allocating threads stack doesn't get unwound in the meantime.  Its an easy mistake to make.

## The pImpl Solution

One solution is to use the pImpl idiom.  If Person was actually a Proxy for a shared pointer, we wouldn't have to remember the 'rules' of how Person can be used in a concurrent environment.

There are downsides to the pImpl idiom:

* The member data associated with the Person instance will now always be heap allocated, even if the instance is declared on the stack.  As outlined above, most uses might have required heap allocation anyway, so maybe this is not a major problem, but it might not be the best fit for all cases.

* Copying an instance of Person only copies the underlying shared pointer, not the member data.  So assignment of a Person instance results in two Person instances pointing to the same member data.  It's the same problem as assigning one shared pointer to another, except we've just hidden the fact it happens.

So what does Person look like with the pImpl idiom employed?






