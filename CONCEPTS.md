# Concepts Used in Implementing this Program

Alright, so after about an hour of pouring through the depths of the internet, I've decided to compile what I've found... although some of the resources I found were pretty good, I think its best I organized the stuff they were saying in one document (this file)... do note that some of the resources are linked below and you can click on them if you think my explanation is a bit lacking (I didn't check youtube though, and if you find some good videos there, that would be helpful too)... Anyway, here is a journey through some of the concepts the implementation uses; I've included some pertinent images to help visualize some things, although its best to check them out *after each chapter*.

## 1. Compiling a C Program

Before we begin, let's just have a short refresher on how a C program is compiled (a good overview [here](https://www.prepbytes.com/blog/c-programming/compilation-process-in-c/)). Whenever you compile a C program, the compiler actually does a few things under the hood.

<p style="text-align:center;">
  <img src="https://static.packt-cdn.com/products/9781789801491/graphics/C11557_01_01.jpg">
</p>

### 1.1 Preprocessing
The first thing it does is check the file for **preprocessor directives** (more info [here](https://www.geeksforgeeks.org/cc-preprocessors/)). These are
basically any lines that begin with the `#` symbol. In the case of `#include`, the preprocessor replaces the line with the file we want to import. With `#define`, on the other hand, the preprocessor replaces instances of the name we defined with the code we associated with that name. So something like 

```C
#define AGE 69
```

means all instances of `AGE` will be replaced with `69` in our C file. Essentially, you can think of the preprocessor as just some guy who edits your original code. It just modifies your what you have so that all lines with a `#` are replaced by whatever they're supposed represent. 

### 1.2 Compiling the C File to an Object File
After this is done, the compiler can then turn that modified C file into instructions your computer can understand. The resulting file containing these instructions is called an **object file**. But depending on whether or not your original C file imports other libraries, the compiler might have to combine more than one of these object files.

You see, whenever you have something like `#include <stdio.h>`, what you're actually including is a file containing just **function prototypes**. A function prototype is just a function declaration without the function body. So stuff like

```C
// A function that does something
void doSomething(int myNumber);

// A function that returns the age of a person
int returnAge(struct Person *person);
```

are function prototypes (they do not contain the code that the function actually executes). These are the only things appended to your original C file when the preprocessor replaces the `#include` line. 

So where are the actual function implementations? Where's the code we need to run the functions? That's where the linking process comes in. 

### 1.3 Linking the Object Files

Because turning a C file into an object file is a rather tedious process, most of the libraries we use (the standard library included) have already had their C files turned into object files before shipping them out for us to use. The only thing left to do would be to combine those object files together, based on what we need. 

Usually, the compiler is smart enough to know which object files to link. Every time you use a standard library using the `#include` directive, the compiler automatically knows which object file is associated with the header file you included. So although header files are only supposed to contain function prototypes, the implementations of these functions (which are already object files at this point) are linked to your program during this step.

You can also think of it this way: when we include header files, what we're basically doing is telling the compiler the functions we'll be using; additionally, the header file also tells us how to use those functions (through their prototypes). But the actual code needed to run those functions isn't provided to our program up until after the compilation process. After all, those files can be pretty big, and importing then compiling those functions *every time we need them* is rather wasteful, as opposed to using their object files (which have already been compiled for us).

Technically speaking, there's a step before linking called **assembling**, but we don't need to know that here. Just know that at this point in time, the C file has effectively been transformed into something that can run on your computer.

> **Chapter 1: Compiling a C Program**
> 
> ***How this Concept Relates to the Implementation***
>
> #### 1. Why are Our Header Files Chonky AF? 
> You might notice that most of our header files (the files with .h; more nuances about them [here](https://gribblelab.org/teaching/CBootCamp/12_Compiling_linking_Makefile_header_files.html)) include more than just the function prototypes: they include the function definitions too. If header files are only supposed to tell us how to use a function, why are we doing this?
>
> Sadly, whenever we have our own custom header files, the compiler isn't smart enough to figure out which object files to link to our program. This means that when compiling, we have to manually specify what files to link. This can make our compile command look nasty with more header files, and it would probably look like this:
>
> 
```C
gcc -o main main.c header1.c header2.c header3.c header4.c ...
```
>
>where the `header.c` files represent the function implementations of the `header.h` files we have in `main.c`. To avoid annoying ourselves (and sir), we will be placing the function implementations with their definitions on all our header files. This essentially means that the preprocessor combines all the code in all our files into one big C(hungus) file, which then gets turned into a single object file.
>
> #### 2. What are the `#ifndef` Stuff for?
>
> At the beginning of every header file we have, you might notice something like a
>
```C
#ifndef HEADER_FILE_NAME_
#define HEADER_FILE_NAME_
```
> followed by one of these at the end of the file:
```C
#endif
```
> These are what we call **inclusion guards**. What this basically does is it encloses the entire code in our file within a massive if statement. Everything between the `#ifndef` and the `#endif` is included by the preprocessor *only when* `HEADER_FILE_NAME_` is not defined.
> 
> Why is this important? When we're coding with a lot of header files, it's really common for us to include the same header file in multiple places. Consider this example:
```C
/**
 * This is header1.h
 **/
#include <stdio.h>

// Let's pretend we have some other code here
```

```C
/**
 * This is header2.h
 **/
#include <stdio.h>

// This is another file which also uses stdio.h
```
> Now let's say we import both of these file in `main.c`.
```C
#include "./header1.h"
#include "./header2.h"
```
> Oops! The preprocessor replaces both of these with the respective contents of the header files, and we end up with two `#include <stdio.h>` in our big C(hungus) file! In other words, there's an impostor among us.
>
> Jokes aside, people realized this could be a problem so they created these things called inclusion guards. Basically, the first time we encounter `#include <stdio.h>` above, it's replaced by all the code associated with stdio.h **AND** it does a `#define` on its inclusion guard. Because of this, any of the succeeding times we encounter `#include <stdio.h>`, the `#ifndef` on its inclusion guard will then return false, and so the code of `<stdio.h>` won't be copied more than once. If it didn't do this, the compiler would probably throw an error telling us its confused why the hell we have two `printf()` functions in our code.

## 2. Program Memory

Understanding the how memory is used by a program is essential not only to be able to code better in C but also to be able to find bugs much faster (for a succint overview, check [this](https://craftofcoding.files.wordpress.com/2015/12/stackmemory31.jpg)).

<p style="text-align:center;">
  <img src="https://craftofcoding.files.wordpress.com/2015/12/stackmemory4.jpg?w=450">
</p>

### 2.1 The Call Stack

In almost all programming languages, a **call stack** is implemented to help keep track of the **functions** (some say **routine** is a better descriptor) we're running. You can think of a function as a group of instructions we've packaged together, alongside all the variables and parameters they need to run. These variables, parameters and instructions are sometimes called the **execution context** of a function.

So how exactly does a call stack work? Every time we call a function, we're essentially saying "hey, let's put ourselves inside this function and just consider everything *it* has for now; we're not really concerned with anything else outside, we'll just finish everything we have inside first". Everything the function needs to do its thing is allocated for it by the program and added on top of the call stack. Only the top of the call stack is accessible at any given time, which makes sense since the function of utmost priority (the one we called last) should be what the program should aim to finish first, before it can resume executing the other stuff underneath it.

Once the current function is done executing, the top of the call stack is "popped" and everything that was allocated for the it is removed. We don't need those things anymore, and it's time to put ourselves back into the context of the previous function.

This is a really convenient structure for managing the memory of a program. However, when we start to deal with more complicated data types like structs and objects, things get a little inefficient. 

As a simple example, let's say we wanted to create a function that returned a string with a few hundred characters. The function could work just as well returning the actual contents of the string, and the program would store the return value in a new variable. But as our objects get larger in size, this becomes a very impractical application as copying a few thousand bytes every time we want to move objects around is a very bad idea.

### 2.2 Heap Memory

Heap Memory is a nifty solution to the previous dilemma. It allows us to allocate memory for objects outside of the call stack, where we won't have to worry about our objects disappearing after the function that created them exits. Passing around a pointer to the allocated memory can then make the program a more efficient at handling data.

In C, there are only two functions that allow us to allocate memory in the heap: `malloc()` and `calloc()`. Both of these functions return *a pointer* to the allocated memory. But because C is a baby, we have to tell it to deallocate the memory once we're done using it with `free()`. The `free()` function accepts a pointer to the memory we wish to deallocate. All these functions are available in the `<stdlib.h>` header.

As a sidenote, here's an important reminder of why `free()` exists. Let's say we do the following:

```C
char *charPointer = calloc(1, sizeof(char));        // Allocate memory for 1 character
char *charOtherPointer = calloc(1, sizeof(char));   // Allocate memory for another single character

*charPointer = 'a';
*charOtherPointer = 'b';
...
```

Currently, our pointers refer to different locations in memory. But if we do:
```C
charPointer = charOtherPointer;
```
Both of the pointers are pointing to the location of `'b'` now. What happened to the original character `'a'`? It's still in the heap memory! What the heck? But I don't have a pointer to it anymore? How am I going to get it back??

The answer is, you can't get it back. It's gone forever. You wasted memory. This is called a **memory leak**.

Whenever you allocate memory using `calloc()` or `malloc()` be sure to call `free()` before you lose track of the pointer. Otherwise, your progam will keep using up more memory which you won't be capable of freeing, at least not until you finally decide to terminate your program.

If some things are still a bit fuzzy, [here's a short thread](https://www.reddit.com/r/AskProgramming/comments/12kr0pp/comment/jg3l4ge/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button) I found with some neat insights into program memory.

> **Chapter 2: Program Memory**
> 
> ***How this Concept Relates to the Implementation***
>
> #### 1. Why tf are there so Many `calloc()` Calls Everywhere?
> A lot of the structs we have in our program have four very specific functions associated with them. What we're actually trying to do is to emulate object-oriented programming... in C! C is such a pussy that it doesn't know what OOP is. Let's go through an example of how we try to do OOP in C (it's not true OOP because we don't have methods (aka functions) tied to the structs, but let's just pretend it is).
>
> Let's say we have the following struct definition for a "class" called Person
```C
typedef struct Person {
  int age;
  char name[32];
} Person;
```
> The first two functions help us (1) allocate memory for an instance of the struct in heap memory and (2) initialize the memory with the values we want it to have.
```C
Person *Person_new() {
  Person *pPerson = calloc(1, sizeof(*pPerson));
  return pPerson;
}

Person *Person_init(Person *this, int age, char name[]) {
  this->age = age;            // Copy the age
  strcpy(this->name, name);   // Assume that we have <string.h> up there

  return this;
}
```
> Note that we use the keyword `this` to refer to the instances we pass because this is usually the keyword used by OOP languages to refer to current the instance the function is acting on.
>
> Also, the `->` operator works just like a `.`, but for pointers. The two pieces of code are equivalent:
```C
// Assuming `this` is a *pointer* to an instance
this->member;

// is the same as
(*this).member;
```
> Although as you can see, the first is clearly superior because it uses two less characters!
> Now when we're actually working with our classes, we don't want to have to call `Person_new()` then `Person_init()` each time. It's best we merge these two processes under a single function. This brings us to
```C
Person *Person_create(int age, char name[]) {
  return Person_init(Person_new(), age, name);
}
```
> This makes our lives way easier and keeps things just a little cleaner. At this point, I caught myself asking "why don't we just put the code of the `Person_new()` and `Person_init()` functions inside the `Person_create()` function? That way we don't have to write the `_new()` and `_init()` stuff for each class." While this might sound like a good way to save me some time typing a few extra lines, I feel like it's better separating out these two processes into their own functions anyway because it makes the code much more clear, and in the case that the `_init()` of an object does a lot of its own stuff, the `_create()` function does a good job of abstracting all that for us. We might also want to call `_new()` without calling `_init()`; this has happened before (though I don't recall where I needed to do that).
>
> The last important function is the `_kill()` function. 
```C
void Person_kill(Person *this) {
  free(this);
}
```
> All this actually does is it calls `free()` on the provided pointer, effectively deallocating the memory of the instance. While we could just call `free()` instead of using the `_kill()` functions, some classes actually have other stuff to do when inside their `_kill()` functions, so its best to use this rather than calling `free()` directly on an instance.
>
> #### 2. Beware of Hidden `malloc()` and `calloc()` Calls
> Just a quick warning before we move on to the next chapter: some standard library functions use `malloc()` and `calloc()`, and unless they say otherwise, it's important you call `free()` on these too. If you don't, baby C is gonna keep eating RAM... Johnny, Johnny! Yes papa? Eating RAM? You get the idea.
>
> How do you know if a function uses `calloc()` or `malloc()`? Fear not! The world wide web is full of documentation on basically everything, so you can easily know if a function uses one of these things.

## 3. Threads and Mutexes

When you run a C program, a single **thread** of execution is created wherein all the instructions you provide execute one after the other. If one process takes too long to finish, everything else the program has to do gets delayed. Unfortunately, once your program gets large enough, you can't avoid having to deal with one of these **blocking processes** that clogs up the whole line of execution. It's like those shared dormitory bathrooms in the morning: there's always that one mf who takes an hour in the shower.

### 3.1 Threads

So how do we deal with these processes? Thankfully, we have threads. Threads allow us to create another path of execution, so to speak, that can let our program do other things on the side while the main process goes on. Remember the stack and heap we talked about? Whenever we create a thread, we actually create a new call stack specifically for that thread. That way, it can execute its own sequence of functions without disturbing the main process. However, no new heap is created. All threads within the same program share the heap memory, which is a really powerful thing! This allows the threads to be able to talk to each other, and is another reason to use `calloc()` and `malloc()` in our program.

But what if two threads want to modify the same chunk of heap memory at the same time?

### 3.2 Mutexes

A mutual exclusion (more commonly known as a mutex) can help us prevent this from happening (I think the analogy [here](https://www.reddit.com/r/explainlikeimfive/comments/29prmj/eli5_in_computer_science_what_is_a_mutex/) is pretty nifty; and no, I didn't realize until after I saw this post that I used a similar analogy above LMAO). Think of a mutex as a lock that restricts access to a memory location. Whenever a thread wants to modify a piece of memory, it first has to ask the mutex associated with that memory whether or not it is currently locked. If it's free, the mutex locks itself for the thread and gives it exclusive access to the resource. Otherwise, the thread is sad and either has to (1) wait for it to become available or (2) carry on doing something else instead.

### 3.3 Implementing Threads and Mutexes

Now you might notice that I haven't provided any code explaining how to implement threads and mutexes. That's because **C99** doesn't support threads and mutexes out of the box. I was about to pull a big sad until I heard that **Windows** supports threads and mutexes. Not only that, but **Unix** (basically Linux systems) also support these things. In other words, even though C99 doesn't give us a direct way to implement them, the operating system that our program runs in can! 

How exactly do we access these functionalities though? Well, it depends on whether you're using Windows or Linux. For the sake of simplicity, I'll explain how to go about this on Windows.

When we're on Windows, the operating system actually provides us a way to access its features when we're building our own programs. These features are available to us in C when we include stuff like the `<windows.h>` header file among other things. 

To use threads and mutexes, we include the following files:
```C
#include <windows.h>
#include <conio.h>
#include <process.h>
```
after which the following functions become available to us:
```C
CreateMutexA();         // Allows us to create a mutex for a memory resource
ReleaseMutex();         // Allows us to unlock a mutex

_beginthread();         // Allows us to start a thread
WaitForSingleObject();  // Allows us to wait for a thread to end or smth else

...
```
I won't list all the functions here, but Microsoft provides some documentation about all the stuff available in `<windows.h>` and their other stuff (I don't recommend you check it out though, they do a bad job of explaining stuff LMAO). If you have more questions about these, I can try my best to answer them for you.

The process of doing this in Linux systems is pretty much the same, although the name of the functions are different because the people who made Windows are not the people who made Linux. I'll get more into why this is a massive pain in the ass in a later chapter; for now just know that although a C standard exists which is supposed to make C behave the same anywhere, this is not always the case. Additionally, things not being covered by the standard means we have to implement some stuff ourselves, like we did with threads.

> **Chapter 3: Threads and Mutexes**
> 
> ***How this Concept Relates to the Implementation***
>
> #### 1. `utils.thread.unix.h` and `utils.thread.win.h`?
> As I explained earlier, we will have to implement the things not covered by the C standard ourselves. Windows and Unix-based systems (which include Linux and Max) have their own different ways of implementing threads, and these are available to us when we include the header files provided by the operating system (these are not really external libraries; usually external libraries are installed by the programmer for them to use in their projects, but these are readily available in the operating system).
>
> On Windows we have `<windows.h>` to help us deal with threads. On Unix-based systems, it's `<pthreads.h>`. Fun fact, in C11, `<pthreads.h>` was actually adopted and became part of the C standard, so we wouldn't have had to do this if we were allowed to use a newer version of C.
>
> A recurring theme of using C to build programs (especially C99) is that a lot of things aren't covered by the standard. That's what you get for using baby C I guess.
>
> #### 2. But Why Do We Need Threads in *Our* Program?
> I ask myself this as I lay awake in bed after having spent a week trying to implement threads for our program using nothing but the shtty documentation of Windows and Unix. "AHAHA!" I say... jk. Threads are pretty cool because we get to do things like event handling (which I'll explain in the next chapter). Also, we can add animations (!!) and a neat timer (!!) to our program now :D
>
> #### 3. Why Did You Wrap the Thread and Mutex Functions in Your Own Class?
> You might notice that I created my own classes (the Thread and Mutex classes) which stores a reference to an actual thread an and actual mutex. This is also true for the other functions (for example, we have our own `Mutex_unlock()` which wraps around `ReleaseMutex()` in Windows and `pthread_mutex_unlock()` on Unix). Why did I have to wrap those objects and functions with my own stuff?
>
> The reason again has to do with the fact that Unix and Windows are not friends. Because using threads on the two operating systems forces us to use different functions, we want to (as much as possible) wrap these functions and objects in our own class so that they have the same name, regardless of whether or not we're on Windows or Unix. That way, we don't have to worry about calling two different functions each time. Consider the code below:
>
```C
// You are in Windows
#ifdef _WIN32
#include "./win/utils.thread.win.h"

// Not in Windows
#else
#include "./unix/utils.thread.unix.h"
#endif
```
> `_WIN32` is something that is automatically defined by the Windows operating system. When we see it's defined, we can be sure that we're on Windows. Otherwise, we're not (and probably on Linux). This is neat, because we can check for what operating system we're currently on, and based on that, include the appropriate file with the code for that operating system.
>
> Now because all the functions in `utils.thread.win.h` and `utils.thread.unix.h` have the same name (these are the functions / classes we used to wrap around the differences between the Windows and Unix stuff), we can call something like `Mutex_unlock()` (which ***we*** defined), and it won't matter whether or not `utils.thread.win.h` or `utils.thread.unix.h` was included because both files defined this function (just with different stuff inside the definitions).

## 4. Callbacks

## 5. Events

## 6. Reducing Data Types: It's All Bits! 

## 7. Miscellaneous Ideas

### 7.1 Why We Use a Buffer Class

### 7.2 What are ANSI Escape Sequences

### 7.3 What is `<windows.h>` and `<conio.h>`

### 7.4 The Problem with C: Compatibility

> why we have our main file compiling the game

### 8. Regarding Our Implementation

There are actually some more things I want to explain to you regarding our implementation, but these things relate more to how our program is designed rather than actual CS concepts... I'd personally prefer to explain these in person, and I'll try to do that within the week after our midterms LMOO. Anyhow, thank you for listening to my TED Talk. If you have any questions, fear not! I will try to answer them :D

