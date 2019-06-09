//
// Created by Alon Lib on 06/06/2019.
//

#include <cstdio>
#include <assert.h>
/** Run the test 4 times, and each time change the _TEST_NUMBER definition.
 * _TEST_NUMBER should be: 1 or 2 or 3 or 4.
 */
#define _TEST_NUMBER 0


#if (1 == _TEST_NUMBER)
#include "malloc_2.cpp"
#else
#include "malloc_3.cpp"
#endif

void malloc2_test_01() {

    // malloc
    int *ten = (int *) malloc(sizeof(int) * 10);
    assert(ten);
    for (int i = 0; i < 10; i++) {
        ten[i] = 10;
    }
    int *five = (int *) malloc(sizeof(int) * 5);
    assert(five);
    for (int i = 0; i < 5; i++) {
        five[i] = 5;
    }

    for (int i = 0; i < 10; i++) {
        assert(ten[i] == 10);
    }
    for (int i = 0; i < 5; i++) {
        assert(five[i] == 5);
    }

    // calloc
    int *three = (int *) calloc(3, sizeof(int));
    assert(three);
    for (int i = 0; i < 3; i++) {
        assert(three[i] == 0);
    }

    // helpers
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 3);
    assert(_num_allocated_bytes() == sizeof(int) * 18);
    assert(_num_meta_data_bytes() == _size_meta_data() * 3);

    // realloc
    int *ninety = (int *) realloc(ten, sizeof(int) * 90);
    for (int i = 0; i < 90; i++) {
        ninety[i] = 90;
    }
    assert(ninety);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 10);
    assert(_num_allocated_blocks() == 4);
    assert(_num_allocated_bytes() == sizeof(int) * 108);
    assert(_num_meta_data_bytes() == _size_meta_data() * 4);

    int *sixty = (int *) realloc(NULL, sizeof(int) * 60);
    assert(sixty);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 10);
    assert(_num_allocated_blocks() == 5);
    assert(_num_allocated_bytes() == sizeof(int) * 168);
    assert(_num_meta_data_bytes() == _size_meta_data() * 5);

    // order so far: ten(freed), five, three, ninety, sixty
    // free & malloc
    free(ninety);
    int *eleven = (int *) malloc(sizeof(int) * 11);
    assert(eleven == ninety);
    for (int i = 0; i < 11; i++) {
        eleven[i] = 11;
    }
    for (int i = 11; i < 90; i++) {
        assert(ninety[i] == 90);
    }

    // order so far: ten(freed), five, three, ninety(eleven), sixty
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 10);
    assert(_num_allocated_blocks() == 5);
    assert(_num_allocated_bytes() == sizeof(int) * 168);
    assert(_num_meta_data_bytes() == _size_meta_data() * 5);

}

void malloc3_test_01() {

    // malloc
    int *ten = (int *) malloc(sizeof(int) * 10);
    assert(ten);
    for (int i = 0; i < 10; i++) {
        ten[i] = 10;
    }
    int *five = (int *) malloc(sizeof(int) * 5);
    assert(five);
    for (int i = 0; i < 5; i++) {
        five[i] = 5;
    }

    for (int i = 0; i < 10; i++) {
        assert(ten[i] == 10);
    }
    for (int i = 0; i < 5; i++) {
        assert(five[i] == 5);
    }

    // calloc
    int *three = (int *) calloc(3, sizeof(int));
    assert(three);
    for (int i = 0; i < 3; i++) {
        assert(three[i] == 0);
    }

    // helpers
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 3);
    assert(_num_allocated_bytes() == sizeof(int) * 18);
    assert(_num_meta_data_bytes() == _size_meta_data() * 3);

    // realloc
    int *ninety = (int *) realloc(ten, sizeof(int) * 90);
    for (int i = 0; i < 10; i++) {
        assert(ninety[i] == 10);
    }
    for (int i = 0; i < 90; i++) {
        ninety[i] = 90;
    }
    assert(ninety);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 10);
    assert(_num_allocated_blocks() == 4);
    assert(_num_allocated_bytes() == sizeof(int) * 108);
    assert(_num_meta_data_bytes() == _size_meta_data() * 4);

    int *sixty = (int *) realloc(NULL, sizeof(int) * 60);
    assert(sixty);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 10);
    assert(_num_allocated_blocks() == 5);
    assert(_num_allocated_bytes() == sizeof(int) * 168);
    assert(_num_meta_data_bytes() == _size_meta_data() * 5);

    // order so far: ten(freed), five, three, ninety, sixty
    // free & malloc
    free(ninety);
    int *eleven = (int *) malloc(sizeof(int) * 11);
    assert(eleven >= ninety);
    assert(eleven <= (void *) ((long) ninety + 79 * sizeof(int)));

    for (int i = 0; i < 11; i++) {
        eleven[i] = 11;
    }
    for (int i = 11 + _size_meta_data() * sizeof(int); i < 90; i++) {
        assert(ninety[i] == 90);
    }

    // order so far: freed(10), five, three, eleven, freed(79-data_size), sixty
    assert(_num_free_blocks() == 2);
    assert(_num_free_bytes() == sizeof(int) * (10 + 79) - _size_meta_data());
    assert(_num_allocated_blocks() == 6);
    assert(_num_allocated_bytes() == sizeof(int) * 168 - _size_meta_data());
    assert(_num_meta_data_bytes() == _size_meta_data() * 6);

    int *old_ten = ten;
    ten = (int *) malloc(sizeof(int) * 10);
    assert(ten == old_ten);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(int) * 79 - _size_meta_data());
    assert(_num_allocated_blocks() == 6);
    assert(_num_allocated_bytes() == sizeof(int) * 168 - _size_meta_data());
    assert(_num_meta_data_bytes() == _size_meta_data() * 6);
    // order so far: ten, five, three, eleven, freed(79-data_size), sixty

}

void malloc3_test_02() {

    long long *tens[11];

    for (int i = 10; i < 101; i += 10) {
        tens[i / 10] = (long long *) malloc(sizeof(long long) * i);
    }
    for (int i = 1; i < 11; i++) {
        for (int j = 0; j < i; j++) {
            tens[i][j] = j;
        }
    }
    for (int i = 1; i < 11; i++) {
        for (int j = 0; j < i; j++) {
            assert(tens[i][j] == j);
        }
    }

    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 10);
    assert(_num_allocated_bytes() == sizeof(long long) * 550);
    assert(_num_meta_data_bytes() == _size_meta_data() * 10);

    free(tens[5]);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == sizeof(long long) * 50);
    assert(_num_allocated_blocks() == 10);
    assert(_num_allocated_bytes() == sizeof(long long) * 550);
    assert(_num_meta_data_bytes() == _size_meta_data() * 10);

    for (int i = 1; i < 11; i++) {
        if (i != 5 && i != 8) free(tens[i]);
    }
    // order: free(280+6*data), 80, free(190+data)
    assert(_num_free_blocks() == 2);
    assert(_num_free_bytes() ==
           sizeof(long long) * 470 + 7 * _size_meta_data());
    assert(_num_allocated_blocks() == 3);
    assert(_num_allocated_bytes() ==
           sizeof(long long) * 550 + 7 * _size_meta_data());
    assert(_num_meta_data_bytes() == _size_meta_data() * 3);

    free(tens[8]);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() ==
           sizeof(long long) * 550 + 9 * _size_meta_data());
    assert(_num_allocated_blocks() == 1);
    assert(_num_allocated_bytes() ==
           sizeof(long long) * 550 + 9 * _size_meta_data());
    assert(_num_meta_data_bytes() == _size_meta_data() * 1);

}

void malloc3_test_03() {

    assert(_num_meta_data_bytes() % 4 == 0); // problem 3 check

    void *huge = (void *) malloc(1000);
    assert(huge);
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 1);
    assert(_num_allocated_bytes() == 1000);
    assert(_num_meta_data_bytes() == _size_meta_data() * 1);
    void *tiny = (void *) malloc(31); // (problem 4)
    free(huge);
    assert(tiny);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 1000);
    assert(_num_allocated_blocks() == 2);
    assert(_num_allocated_bytes() == 1032);
    assert(_num_meta_data_bytes() == _size_meta_data() * 2);

    // fits just right (problem 1 test for exactly 128 free bytes is okay for split)
    void *mid = (void *) malloc(1000 - 128 - _size_meta_data());
    assert(mid >= huge && mid <= (void *) ((long) huge + 872));
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 128);
    assert(_num_allocated_blocks() == 3);
    assert(_num_allocated_bytes() == 1032 - _size_meta_data());
    assert(_num_meta_data_bytes() == _size_meta_data() * 3);

    free(mid); // free place should unite problem 2 and become 1000 again
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 1000);
    assert(_num_allocated_blocks() == 2);
    assert(_num_allocated_bytes() == 1032);
    assert(_num_meta_data_bytes() == _size_meta_data() * 2);

    // doesnt fit (problem 1 test for exactly 124 free bytes shouldn't split)
    mid = (void *) malloc(1000 - 124 - _size_meta_data());
    assert(mid == huge);
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 2);
    assert(_num_allocated_bytes() == 1032);
    assert(_num_meta_data_bytes() == _size_meta_data() * 2);

    free(tiny); // a wilderness block (problem 3)
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 32);
    assert(_num_allocated_blocks() == 2);
    assert(_num_allocated_bytes() == 1032);
    assert(_num_meta_data_bytes() == _size_meta_data() * 2);

    tiny = (void *) malloc(33); // (problem 3)
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 2);
    assert(_num_allocated_bytes() == 1036);
    assert(_num_meta_data_bytes() == _size_meta_data() * 2);
}

int main() {
    printf("Hi there!\n");
    //printf("%l",(long)sizeof(allocated_meta_data));

    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 0);
    assert(_num_allocated_bytes() == 0);
    assert(_num_meta_data_bytes() == 0);

    /// run the tests each one separately because list uses global vars
    /// that means the test needs to run 4 times with different _TEST_NUMBER

    switch (_TEST_NUMBER) {
        case 1:
            malloc2_test_01();
            break;
        case 2:
            malloc3_test_01();
            break;
        case 3:
            malloc3_test_02();
            break;
        case 4:
            malloc3_test_03();
            break;
        default:
            printf("Invalid definition of macro \"_TEST_NUMBER\",\nshould be: 1/2/3/4.");
            return 1;
    }

    printf("Success for test: %d.\n", _TEST_NUMBER);
    return 0;
}