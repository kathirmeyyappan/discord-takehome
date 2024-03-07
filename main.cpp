#include <cstdint>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <criterion/criterion.h>


// 1. Implement a function to identify a duplicate integer in an unsorted array
// of integers
// * `input` contains exactly N+1 numbers
// * `input` elements are integers in the domain [1, N]
// * `input` contains all integers in the domain [1, N] at least once

/* Doing this in O(n) time with a hashmap/set is trivial, but to do this task in
O(1) space, I utilize the fact that the sum of numbers from 1 to n is constant,
and finding the duplicate is simply a matter of subtracting this sum from 
the accumulated sum of our vector. */
int findDuplicate(std::vector<int> const& input) {
    unsigned long long sum = 0;
    int n = input.size();
    for (int i = 0; i < n; i++) {
        sum += (unsigned long long)input[i];
    }
    unsigned long long expected = (n * (n - 1) / 2);
    return (int)(sum - expected);
}



// 2. Implement a growable integer stack (without using container library) that satisfies the following requirements:
// * `push` operation which adds a given value to the top
// * `pop`  operation which returns and removes the value at the top
// * `peek` operation which returns the value at the top
// * `size` operation which returns the count of values

/* This is the Node struct used to implement the (linked list) stack. */
struct Node {
    int val;
    Node* nxt;

    // constructor
    Node(int v, Node *next) : val(v), nxt(next) {}
};

/* I implemented the stack with a linked list of integers in order to
avoid using containers. */
class GrowableIntegerStack {
private:
    // linked list head
    Node *top;
    unsigned int len;

public:
    // constructor
    GrowableIntegerStack() : top(nullptr), len(0) {}

    // destructor
    ~GrowableIntegerStack() {
        while (top) {
            Node *hold = top;
            top = top->nxt;
            delete hold;
        }
    }

    // add new head in front of linked list
    void push(int x) {
        Node *new_top = new Node(x, top);
        top = new_top;
        len++;
    }    

    // return head val from linked list and move head to next node
    int pop() {
        if (!top) {
            throw std::runtime_error("Cannot pop from empty stack.");
        }
        int res = peek();
        Node *old_top = top; 
        top = top->nxt;
        delete old_top;
        len--;
        return res;
    }

    // get value of current head
    int peek() {
        if (!top) {
            throw std::runtime_error("Cannot peek empty stack.");
        }
        return top->val;
    }

    // simply return running count
    unsigned int size() {
        return len;
    }    
};



// 3. Implement a blitting function which supports color-keyed transparency.
unsigned int const COLOR_KEY{0xFF000000};


struct PixelBuffer {
 uint32_t *pixels;
 int width;
 int height;
 // added constructor for test convenience
 PixelBuffer(uint32_t *ps, int w, int h) : pixels(ps), width(w), height(h) {}
};


// Copies the entire image from `src` into a destination buffer `dest`.
// The pixel buffers have a top-left origin and are row-major.
// `offsetX` and `offsetY` denote the origin within `dest` where `src` should be
// copied.
// Any pixel that exactly matches `COLOR_KEY` should be skipped.
// You may assume input buffers are pre-allocated and sufficiently large to
// complete the requested operation.

/* This implementation just iterates through src, editing the corresponding values
in dest based on if the src value is the non-blit color or not. */
void blit(PixelBuffer const* src, PixelBuffer* dest, size_t offsetX, size_t offsetY) {
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            uint32_t src_argb = src->pixels[i * src->width + j];
            if (src_argb != COLOR_KEY) {
                size_t pos = ((offsetY + i) * dest->width) + (offsetX + j);
                dest->pixels[pos] = src_argb;
            }
        }
    } 
}



/* TESTS FOR ALL METHODS BELOW 

I have written tests for each of the three parts to test functionality based
on my understanding of the problem statements.

This whole program was compiled with:
    clang++ -g -o main main.cpp -lcriterion

^ this just runs all the tests

Note: All tests are passing. */


// findDuplicate tests

Test(findDuplicate, fd1) {
    std::vector<int> lst = {1, 3, 6, 3, 2, 4, 5};
    cr_assert(findDuplicate(lst) == 3);
}

Test(findDuplicate, fd2) {
    std::vector<int> lst = {1, 1};
    cr_assert(findDuplicate(lst) == 1);
}

Test(findDuplicate, fd3) {
    std::vector<int> lst;
    for (int i = 1; i < 100000; i++) {
        lst.push_back(i);
    }
    lst.push_back(1234);
    cr_assert(findDuplicate(lst) == 1234);
}


// GrowableIntegerStack tests

Test(GrowableIntegerStack, gis_test_1) {
    GrowableIntegerStack stack = GrowableIntegerStack();

    // push test
    stack.push(42);
    cr_assert_eq(stack.size(), 1);

    // pop test
    int p = stack.pop();
    cr_assert_eq(p, 42);
    cr_assert_eq(stack.size(), 0);

    // error tests
    cr_assert_throw(stack.peek(), std::runtime_error);
    cr_assert_throw(stack.pop(), std::runtime_error);

    // peek test
    stack.push(100);
    cr_assert_eq(stack.peek(), 100);
}

Test(GrowableIntegerStack, gis_test_2) {
    GrowableIntegerStack stack = GrowableIntegerStack();

    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);

    cr_assert(stack.peek() == 5);
    cr_assert(stack.pop() == 5);
    cr_assert(stack.pop() == 4);
    cr_assert(stack.pop() == 3);

    stack.push(4);
    stack.push(5);

    cr_assert(stack.size() == 4);
}


// blit tests

Test(blit, blit_1) {
    uint32_t d[1] = {
        0xFFA8F81F
    };
    uint32_t s[1] = {
        0xFF000000
    };
    
    PixelBuffer dest = PixelBuffer(d, 1, 1);
    PixelBuffer src = PixelBuffer(s, 1, 1);
    
    blit(&src, &dest, 0, 0);

    uint32_t target[1] = {
        0xFFA8F81F
    };

    bool res = true;
    for (int i = 0; i < dest.width * dest.height; i++) {
        if (dest.pixels[i] != target[i]) {
            res = false;
            break;
        }
    }
    cr_assert(res);
}

Test(blit, blit_2) {
    uint32_t d[1] = {
        0xFFA8F81F
    };
    uint32_t s[1] = {
        0xFF0000FF
    };
    
    PixelBuffer dest = PixelBuffer(d, 1, 1);
    PixelBuffer src = PixelBuffer(s, 1, 1);
    
    blit(&src, &dest, 0, 0);

    uint32_t target[1] = {
        0xFF0000FF
    };

    bool res = true;
    for (int i = 0; i < dest.width * dest.height; i++) {
        if (dest.pixels[i] != target[i]) {
            res = false;
            break;
        }
    }
    cr_assert(res);
}

Test(blit, blit_3) {
    uint32_t d[12] = {
        0xFFA8F81F, 0xFFFFFFFF, 0xFF1112FF,
        0xFF1112FF, 0xFFFFFFFF, 0xFF1112FF,
        0xFFFFFFFF, 0xFFA8F81F, 0xFFFFFFFF,
        0xFF1112FF, 0xFFFFFFFF, 0xFFA8F81F
    };
    uint32_t s[2] = {
        0xFF000000, 
        0xFF0000FF
    };
    
    PixelBuffer dest = PixelBuffer(d, 3, 4);
    PixelBuffer src = PixelBuffer(s, 1, 2);
    
    blit(&src, &dest, 0, 0);

    uint32_t target[12] = {
        0xFFA8F81F, 0xFFFFFFFF, 0xFF1112FF,
        0xFF0000FF, 0xFFFFFFFF, 0xFF1112FF,
        0xFFFFFFFF, 0xFFA8F81F, 0xFFFFFFFF,
        0xFF1112FF, 0xFFFFFFFF, 0xFFA8F81F
    };

    bool res = true;
    for (int i = 0; i < dest.width * dest.height; i++) {
        if (dest.pixels[i] != target[i]) {
            res = false;
            break;
        }
    }
    cr_assert(res);
}

Test(blit, blit_4) {
    uint32_t d[20] = {
        0xFFA8F81F, 0xFFFFFFFF, 0xFF1112FF, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFF1112FF, 0xFFA8F81F,
        0xFFFFFFFF, 0xFFA8F81F, 0xFFFFFFFF, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFFA8F81F, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFF1112FF, 0xFFA8F81F
    };
    uint32_t s[4] = {
        0xFF000000, 0xFF123456,
        0xFF123456, 0xFF000000
    };
    
    PixelBuffer dest = PixelBuffer(d, 4, 5);
    PixelBuffer src = PixelBuffer(s, 2, 2);
    
    blit(&src, &dest, 1, 1);

    uint32_t target[20] = {
        0xFFA8F81F, 0xFFFFFFFF, 0xFF1112FF, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFF123456, 0xFFA8F81F,
        0xFFFFFFFF, 0xFF123456, 0xFFFFFFFF, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFFA8F81F, 0xFFA8F81F,
        0xFF1112FF, 0xFFFFFFFF, 0xFF1112FF, 0xFFA8F81F
    };

    bool res = true;
    for (int i = 0; i < dest.width * dest.height; i++) {
        if (dest.pixels[i] != target[i]) {
            res = false;
            break;
        }
    }
    cr_assert(res);
}