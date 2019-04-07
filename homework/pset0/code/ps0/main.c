#include "bst.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

/* Type: FileData
 * ---------------------------------------------------------------------------
 * A type representing tree data read from the file. It consists of a pair of
 * a number of elements and the elements themselves, in order.
 */
struct FileData {
  size_t num_elems;
  int*   elems;
};

/* Constant: TEST_DIRECTORY
 * ---------------------------------------------------------------------------
 * Name of the directory containing test cases to run. This must end with a
 * slash.
 */
#define TEST_DIRECTORY "tests/"

/* Macro: error(msg)
 * ---------------------------------------------------------------------------
 * Reports an error and terminates the program.
 */
#define error(msg) do_error(msg, __FILE__, __LINE__)
static void do_error(const char* msg, const char* file, unsigned line) {
  fprintf(stderr, "ERROR: %s\nFile: %s\nLine: %u\n", msg, file, line);
  abort();
}

/* Function: read_file(filename)
 * ---------------------------------------------------------------------------
 * Returns the contents of a tree test file as a FileData object. On error,
 * aborts the program.
 */
static struct FileData read_file(const char* test_file) {
  struct FileData result;

  FILE* f = fopen(test_file, "r");
  if (f == NULL) error("Can't open test file.");
  
  /* Read the tree contents into a separate buffer so that we can see whether everything worked
   * properly.
   */
  if (fscanf(f, "%zu", &result.num_elems) != 1) error("Failed to read number of elements from file.");
  
  result.elems = calloc(result.num_elems, sizeof(int));
  for (size_t i = 0; i < result.num_elems; i++) {
    if (fscanf(f, "%d", &result.elems[i]) != 1) error("Failed to read integer from file.");
  }
  
  /* Confirm there's nothing left by trying to read another integer and watching it fail. */
  if (fscanf(f, "%*d") != EOF) error("Unexpected file contents found after end of tree data.");
  
  fclose(f);
  
  return result;
}

/* Function: compare_ints(&i1, &i2)
 * ---------------------------------------------------------------------------
 * Comparison function for integers, for use in qsort.
 */
int compare_ints(const void* lhs, const void* rhs) {
  int left  = *(const int *)lhs;
  int right = *(const int *)rhs;
  
  /* We can't just do right - left, since that might overflow. */
  if (left < right) return -1;
  if (left > right) return +1;
  return 0;
}

/* Function: assert_monotone_increasing(data, size)
 * ---------------------------------------------------------------------------
 * Asserts that the range pointed at by data is monotonically increasing (each
 * element is strictly greater than the previous.)
 */
static void assert_monotone_increasing(const int* data, size_t size) {
  for (size_t i = 1; i < size; i++) {
    if (data[i - 1] >= data[i]) {
      fprintf(stderr, "Elements not strictly increasing: %d >= %d\n", data[i-1], data[i]);
      abort();
    }
  }
}

/* Function: advance(data, size, &index)
 * ---------------------------------------------------------------------------
 * Advances the index value forward in the data range until it either hits the
 * end of the range or encounters a different value. It is assumed that the
 * index is purely before the end of the range, since otherwise this is not
 * mathematically well-defined.
 */
static void advance(const int* data, size_t size, size_t* index) {
  size_t result = *index + 1;
  while (result < size && data[result] == data[*index]) result++;
  
  *index = result;
}

/* Function: assert_sets_equal(data1, size1, data2, size2);
 * ---------------------------------------------------------------------------
 * Checks that the elements pointed at by data1 and data2 are equal to one
 * another, ignoring duplicates. The elements must be in sorted order. If the
 * ranges aren't equal, this function signals an error.
 */
static void assert_sets_equal(const int* data1, size_t size1,
                              const int* data2, size_t size2) {
  /* Loop over the ranges until at least one is exhausted. */
  size_t i1 = 0, i2 = 0;
  while (i1 < size1 && i2 < size2) {
    /* Confirm the elements here match. */
    if (data1[i1] < data2[i2]) {
      fprintf(stderr, "Mismatch found: first range contains %d, second does not.\n", data1[i1]);
      abort();
    } else if (data1[i1] > data2[i2]) {
      fprintf(stderr, "Mismatch found: second range contains %d, second does not.\n", data2[i2]);
      abort();
    }
    
    /* Advance each pointer forward until it either hits the range end or no
     * longer points at an equal value.
     */
    advance(data1, size1, &i1);
    advance(data2, size2, &i2);
  }
  
  /* Both ranges should be empty at this point. */
  if (i1 < size1) {
    fprintf(stderr, "End of second range was encountered before end of first range.\n");
    abort();
  } else if (i2 < size2) {
    fprintf(stderr, "End of first range was encountered before end of second range.\n");
    abort();
  }
}

static void run_test(const char* test_file) {
  printf("Test File: %s\n", test_file);

  struct FileData data = read_file(test_file);
  
  /* Construct a tree from this data. */
  struct Node* root = NULL;
  for (size_t i = 0; i < data.num_elems; i++) {
    insert_into(&root, data.elems[i]);
  }
  
  int* tree_elems = contents_of(root);
  size_t tree_size = size_of(root);
  
  printf("  Tree contains %zu elements:\n", tree_size);
  printf("  [");
  for (size_t i = 0; i < tree_size; i++) {
    printf("%d%s", tree_elems[i], (i == tree_size - 1)? "" : ", ");
  }
  printf("]\n");
  fflush(stdout);
  
  assert_monotone_increasing(tree_elems, tree_size);

  /* Confirm that the tree contents match the list contents, ignoring
   * duplicates. This is much easier to do if we sort the list contents
   * first.
   */
  qsort(data.elems, data.num_elems, sizeof(int), compare_ints);
    
  assert_sets_equal(data.elems, data.num_elems, tree_elems, tree_size);
  
  /* Print out the second-min value. */
  const struct Node* second = second_min_in(root);
  if (second == NULL) {
    printf("  This tree does not have a second-smallest element.\n");
  } else {
    printf("  The second-smallest element in the tree is %d.\n", second->value);
  }
  
  free_tree(root);
  free(data.elems);
  free(tree_elems);
}

int main() {
  DIR* tests = opendir(TEST_DIRECTORY);
  if (tests == NULL) error("Could not open " TEST_DIRECTORY " for reading.");
  
  for (struct dirent* entry; (entry = readdir(tests)) != NULL; ) {
    /* Make sure this isn't a dotted file (hidden file, ., or ..) */
    if (entry->d_name[0] == '.') continue;
    
    char test_file_name[strlen(entry->d_name) + strlen(TEST_DIRECTORY) + 1];
    strcpy(test_file_name, TEST_DIRECTORY);
    strcat(test_file_name, entry->d_name);
    run_test(test_file_name);
  }
  if (errno != 0) error("Error traversing the " TEST_DIRECTORY " directory.");
  closedir(tests);
  
  return 0;
}
