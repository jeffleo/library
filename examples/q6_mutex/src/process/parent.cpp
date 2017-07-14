#include <iostream>
#include <string>

#include "common.h"
#include "cpen333/util.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/mutex.h"
#include "cpen333/process/unlinker.h"
#include "cpen333/process/subprocess.h"

//
// Mutexes (Mut_ual Ex_clusions) are used to provide exclusive access to a shared resource.  A basic mutex has two
// functions:
//     lock() - will block (wait here) until the mutex is found in an unlocked state, and will then lock it to continue
//     unlock() - will unlock, allowing someone else waiting for the mutex to lock it
//
// MAKE SURE TO UNLOCK YOUR MUTEX WHEN YOU ARE DONE WITH IT!! Otherwise, many others may be stuck waiting for it.
// For this reason, a mutex is very rarely used on its own.  It is usually "guarded" using RAII principles with one
// of two classes:
//     std::lock_guard< mutex type > :   locks the mutex in the constructor, unlocks in the destructor
//     std::unique_lock<  mutex type > : locks the mutex in the constructor (or we can "defer" if we like),
//                                       contains methods to lock/unlock the mutex manually
//                                       unlocks the mutex in the destructor
//
// In most cases you should use a lock_guard, unless you need the ability to unlock the mutex manually, in which case
// use a unique_lock.  The ONLY time /not/ to use a lock is if efficiency is a primary concern, and you can guarantee
// that the function won't return and nothing will throw an exception between mutex.lock() and mutex.unlock() calls.
//

int main() {

  // shared memory
  cpen333::process::shared_object<SharedData> data(MUTEX_SHARED_MEMORY_NAME);
  cpen333::process::unlinker<decltype(data)> data_unlinker(data);           // unlink when out of scope

  // mutex
  cpen333::process::mutex mutex(MUTEX_MUTEX_NAME);
  cpen333::process::unlinker<decltype(mutex)> mutex_unlinker(mutex);        // unlink when out of scope

  // Unsafe access, does not use a mutex to protect the shared resource "data"
  std::cout << "Unsafe access to shared variable 'a':" << std::endl;
  auto start = std::chrono::steady_clock::now();
  data->a = 0;
  // run child processes
  cpen333::process::subprocess p1({"./child_unsafe", "1000000"});
  cpen333::process::subprocess p2({"./child_unsafe", "1000000"});
  p1.join();
  p2.join();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << data->a << " , time: " << time.count() << "ms" << std::endl;


  // Safe access, using a mutex to protect the shared resource
  std::cout << "Safe access to shared variable 'a':" << std::endl;
  start = std::chrono::steady_clock::now();
  data->a = 0;
  // run child processes
  cpen333::process::subprocess p3({"./child_safe", "1000000"});
  cpen333::process::subprocess p4({"./child_safe", "1000000"});
  p3.join();
  p4.join();
  end = std::chrono::steady_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << data->a << " , time: " << time.count() << "ms" << std::endl;

  cpen333::pause();

  return 0;
}
