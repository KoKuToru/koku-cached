koku-cached
===========

C++ class that makes it possible to reuse previouse used memory.
Uses C++11.

Simple Example
-----

    for(int i = 0; i < 10000; ++i) {
      std::vector<int> abc;
      //do something with abc
    }
    
This will kill your performance, it would be wiser to use

    std::vector<int> abc;
    for(int i = 0; i < 10000; ++i) {
      abc.clear();
      //do something with abc
    }
    
But in my opinion, sometimes it's not that easy to move it outside.
Like when a loop calls a function.. and in this function you use `std::vector`..
How do you get it outside ?.. simple with `koku::cached` let it stay inside

    for(int i = 0; i < 10000; ++i) {
      koku::cached<std::vector<int>> abc;
      //do something with abc-> or abc.raw()
      abc->clear(); //make sure memory is empty ;)
    }
    
get performance like it's outside. 

Thread safety
-----
Is thread safe, with atomic logic (lock free ?)

Copy safety
-----
Is safe to copy as long as your object (template parameter) is safe to copy..

Memory cleanup ?
-----
Uses a static array of object (template parameter). So should clean up.

TODO
----
Add move-sematic, I am not that sure how "move" works yet.
