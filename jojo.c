#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

typedef enum { false, true } bool;

typedef intptr_t cell;

cell max(cell a, cell b) {
  if (a < b) {
    return b;
  }
  else {
    return a;
  }
}

cell min(cell a, cell b) {
  if (a > b) {
    return b;
  }
  else {
    return a;
  }
}

cell power(cell a, cell n) {
  cell result = 1;
  while (n >= 1) {
    result = result * a;
    n--;
  }
  return result;
}

bool isbarcket(char c) {
  return (c == '(' ||
          c == ')' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}' ||
          c == '"');
}

cell char_to_nat(char c) {
  if (c >= '0' && c <= '9') {
    return (c - '0');
  }
  else if (c >= 'A' && c <= 'Z') {
    return (c - 'A') + 10;
  }
  else if (c >= 'a' && c <= 'z') {
    return (c - 'a') + 10;
  }
  else {
    return 0;
  }
}

typedef char* string;

bool string_equal(string s1, string s2) {
  if (strcmp(s1, s2) == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool nat_string_p(string str) {
  cell i = 0;
  while (str[i] != 0) {
    if (!isdigit(str[i])) {
      return false;
      }
    i++;
  }
  return true;
}

bool int_string_p(string str) {
  if (str[0] == '-' ||
      str[0] == '+') {
    return nat_string_p(str + 1);
  }
  else {
    return nat_string_p(str);
  }
}

cell string_to_based_nat(string str, cell base) {
  cell result = 0;
  cell len = strlen(str);
  cell i = 0;
  while (i < len) {
    result = result + (char_to_nat(str[i]) * power(base, (len - i - 1)));
    i++;
  }
  return result;
}

cell string_to_based_int(string str, cell base) {
  if (str[0] == '-') {
    return - string_to_based_nat(str, base);
  }
  else {
    return string_to_based_nat(str, base);
  }
}

cell string_to_dec(string str) { return string_to_based_int(str, 10); }
cell string_to_bin(string str) { return string_to_based_int(str,  2); }
cell string_to_oct(string str) { return string_to_based_int(str,  8); }
cell string_to_hex(string str) { return string_to_based_int(str, 16); }

typedef void (*primitive)();

typedef cell jo;

typedef struct {
  cell size;
  jo *array;
} jojo;

typedef union {
  cell cell;
  primitive primitive;
  jojo jojo;
} bind;

typedef struct {
  cell index;
  string key;
  jo type;
  bind value;
  cell orbit_length;
  cell orbiton;
} jotable_entry;

jo str2jo (string str);

jotable_entry proto_jotable_entry(cell index) {
  jotable_entry e = {
    .index = index,
    .key = 0,
    .type = str2jo("none"),
    .value.cell = 0,
    .orbit_length = 0,
    .orbiton = 0
  };
  return e;
}

bool jotable_entry_occured(jotable_entry e) {
  return e.key != 0;
}

bool jotable_entry_used(jotable_entry e) {
  return e.type != str2jo("none");
}

bool jotable_entry_no_collision(jotable_entry e) {
  return e.index == e.orbiton;
}

// prime table size
//   1000003   about 976 k
//   1000033
//   1000333
//   100003    about 97 k
//   100333
//   997
#define jotable_size 100003
jotable_entry jotable[jotable_size];
cell jotable_counter = 0;

cell string_to_sum(string str) {
  cell sum = 0;
  cell max_step = 10;
  cell i = 0;
  while (i < strlen(str)) {
    sum = sum + ((unsigned char) str[i]) * (2 << min(i, max_step));
    i++;
  }
  return sum;
}

bool jotable_keyeq(string k1, string k2) {
  return string_equal(k1, k2);
}

cell jotable_hash(string key, cell counter) {
  return (counter + string_to_sum(key)) % jotable_size;
}

char string_area[4 * 1024 * 1024];
cell string_area_counter = 0;

string copy_to_string_area(string str) {
  char *str1;
  cell i = 0;
  str1 = (string_area + string_area_counter);
  while (true) {
    if (str[i] == 0) {
      str1[i] = str[i];
      i++;
      break;
    }
    else {
      str1[i] = str[i];
      i++;
    }
  }
  string_area_counter = i + string_area_counter;
  return str1;
}

// -1 denotes the hash_table is filled
cell jotable_insert(string key) {
  cell orbit_index = jotable_hash(key, 0);
  cell counter = 0;
  while (true) {
    cell index = jotable_hash(key, counter);
    if (!jotable_entry_occured(jotable[index])) {
      key = copy_to_string_area(key);
      jotable[index].key = key;
      jotable[index].orbiton = orbit_index;
      jotable[orbit_index].orbit_length = 1 + counter;
      jotable_counter = 1 + jotable_counter;
      return index;
    }
    else if (jotable_keyeq(key, jotable[index].key)) {
      return index;
    }
    else if (counter == jotable_size) {
      return -1;
    }
    else {
      counter = 1 + counter;
    }
  }
}

// -1 denotes key not occured
cell jotable_search(string key) {
  cell counter = 0;
  while (true) {
    cell index = jotable_hash(key, counter);
    if (!jotable_entry_occured(jotable[index])) {
      return -1;
    }
    else if (jotable_keyeq(key, jotable[index].key)) {
      return index;
    }
    else if (counter == jotable_size) {
      return -1;
    }
    else {
      counter = 1 + counter;
    }
  }
}

string jo2str (cell index);

void jotable_entry_print(jotable_entry entry) {
  printf("%s : ", jo2str(entry.type));
  if (entry.type == str2jo("variable")) {
    printf("%ld", entry.value.cell);
  }
  else if (entry.type == str2jo("primitive")) {
    printf("%ld", entry.value.primitive);
  }
  else if (entry.type == str2jo("function")) {
    printf("%ld ", entry.value.jojo.size);
    printf("[ ");
    cell i;
    for (i=0; i < entry.value.jojo.size; i=i+1) {
      printf("%ld ", entry.value.jojo.array[i]);
    }
    printf("]");
  }
}

void jotable_report_orbit(cell index, cell counter) {
  while (counter < jotable[index].orbit_length) {
    string key = jotable[index].key;
    cell next_index = jotable_hash(key, counter);
    if (index == jotable[next_index].orbiton) {
      printf("  | %ld %s\n", next_index, jotable[next_index].key);
    }
    if (jotable_entry_used(jotable[next_index])) {
      printf("    = ");
      jotable_entry_print(jotable[next_index]);
      printf("\n");
    }
    counter = 1 + counter;
  }
}

void jotable_report() {
  printf("\n");
  printf("- jotable_report\n");
  printf("  : <index> <key> // <orbit-length>\n");
  cell index = 0;
  while (index < jotable_size) {
    if (jotable_entry_occured(jotable[index]) &&
        jotable_entry_no_collision(jotable[index])) {
      printf("  - %ld %s // %ld\n",
             index, jotable[index].key, jotable[index].orbit_length);
      if (jotable_entry_used(jotable[index])) {
        printf("    = ");
        jotable_entry_print(jotable[index]);
        printf("\n");
      }
      jotable_report_orbit(index, 1);
    }
    index = 1 + index;
  }
  printf("  : <index> <key> // <orbit-length>\n");
  printf("\n");
  printf("- used : %ld\n", jotable_counter);
  printf("- free : %ld\n", jotable_size - jotable_counter);
}

void jotable_print() {
  printf("\n");
  printf("- jotable_print\n");
  cell index = 0;
  while (index < jotable_size) {
    printf("  - %ld %s %ld // %ld\n",
           index,
           jotable[index].key,
           jotable[index].value,
           jotable[index].orbit_length);
    index = 1 + index;
  }
  printf("\n");
  printf("- used : %ld\n", jotable_counter);
  printf("- free : %ld\n", jotable_size - jotable_counter);
}

jo str2jo(string str) {
  return jotable_insert(str);
}

string jo2str(cell index) {
  return jotable[index].key;
}

void init_jotable() {
  cell i = 0;
  while (i < jotable_size) {
    jotable[i] = proto_jotable_entry(i);
    i++;
  }
}

jo jojo_area[1024 * 1024];

typedef jo* compiling_stack_t[1024];

compiling_stack_t compiling_stack;
cell compiling_stack_base = 0;
cell compiling_stack_pointer = 0;

void compiling_stack_push(jo* value) {
  compiling_stack[compiling_stack_pointer] = value;
  compiling_stack_pointer++;
}

jo* compiling_stack_pop() {
  compiling_stack_pointer--;
  return compiling_stack[compiling_stack_pointer];
}

void compiling_stack_inc() {
  compiling_stack[compiling_stack_pointer - 1] =
    compiling_stack[compiling_stack_pointer - 1] + 1;
}


jo* compiling_stack_tos() {
  return compiling_stack[compiling_stack_pointer - 1];
}

bool compiling_stack_empty_p() {
  return compiling_stack_pointer == compiling_stack_base;
}

void init_compiling_stack() {
  compiling_stack_push(jojo_area);
}

void here(cell n) {
  jo* pointer = compiling_stack_pop();
  pointer[0] = n;
  compiling_stack_push(pointer + 1);
}

void jotable_set_cell(cell index, cell cell) {
  jotable[index].type = str2jo("variable");
  jotable[index].value.cell = cell;
}

void jotable_set_primitive(cell index, primitive primitive) {
  jotable[index].type = str2jo("primitive");
  jotable[index].value.primitive = primitive;
}

cell jotable_get_cell(cell index) {
  return jotable[index].value.cell;
}

primitive jotable_get_primitive(cell index) {
  return jotable[index].value.primitive;
}

jojo jotable_get_jojo(cell index) {
  return jotable[index].value.jojo;
}

void jotable_test() {
  str2jo("testkey0");
  str2jo("testkey1");
  str2jo("testkey2");
  str2jo("testkey3");
  str2jo("testkey4");

  str2jo("testkey0");
  str2jo("testkey1");
  str2jo("testkey2");
  str2jo("testkey3");
  str2jo("testkey4");

  str2jo("testtestkey0");
  str2jo("testtestkey1");
  str2jo("testtestkey2");
  str2jo("testtestkey3");
  str2jo("testtestkey4");

  str2jo("testtesttestkey0");
  str2jo("testtesttestkey1");
  str2jo("testtesttestkey2");
  str2jo("testtesttestkey3");
  str2jo("testtesttestkey4");

  str2jo("testtesttesttestkey0");
  str2jo("testtesttesttestkey1");
  str2jo("testtesttesttestkey2");
  str2jo("testtesttesttestkey3");
  str2jo("testtesttesttestkey4");

  jotable_set_cell(str2jo("k1"), 1);
  jotable_report();

  jotable_set_cell(str2jo("k1"), 0);
  jotable_report();

  // jotable_print();
}

jo defprim_record[64 * 1024];
cell defprim_record_counter = 0;

void defprim_report() {
  printf("- defprim_report // counter : %ld\n", defprim_record_counter);
  cell i = 0;
  while (i < defprim_record_counter) {
    printf("  %s\n", jo2str(defprim_record[i]));
    i++;
  }
  printf("\n");
}

void k_ignore();
bool used_jo_p(jo index);
jo jo_to_jo_in_module(jo alias_jo);

void defprim(string str, primitive fun) {
  jo index = jo_to_jo_in_module(str2jo(str));
  if (used_jo_p(index)) {
    printf("- defprim can not re-define : %s\n", jo2str(index));
    printf("  it already defined as : %s\n", jo2str(jotable[index].type));
    k_ignore();
    return;
  }
  defprim_record[defprim_record_counter] = index;
  defprim_record_counter++;
  defprim_record[defprim_record_counter] = 0;
  jotable_set_primitive(index, fun);
}

typedef cell argument_stack[1024 * 4];

argument_stack as;
cell as_base = 64;
cell as_pointer = 64;

void as_push(cell value) {
  as[as_pointer] = value;
  as_pointer++;
}

cell as_pop() {
  as_pointer--;
  return as[as_pointer];
}

typedef jo* return_stack[1024 * 4];

return_stack rs;
cell rs_base = 64;
cell rs_pointer = 64;

void rs_push(jo* value) {
  rs[rs_pointer] = value;
  rs_pointer++;
}

jo* rs_pop() {
  rs_pointer--;
  return rs[rs_pointer];
}

void jo_apply(jo jo) {
  if (!jotable_entry_used(jotable[jo])) {
    printf("undefined jo : %s\n", jo2str(jo));
    return;
  }
  cell jo_type = jotable[jo].type;
  if (jo_type == str2jo("primitive")) {
    primitive primitive = jotable_get_primitive(jo);
    primitive();
  }
  else if (jo_type == str2jo("function")) {
    jojo jojo = jotable_get_jojo(jo);
    rs_push(jojo.array);
  }
  else if (jo_type == str2jo("variable")) {
    cell cell = jotable_get_cell(jo);
    as_push(cell);
  }
}

void p_jo_apply() {
  jo_apply(as_pop());
}

void apply(jo* jojo_array) {
  rs_push(jojo_array);
}

void p_apply() {
  apply(as_pop());
}

jmp_buf eval_jmp_buffer;

bool exit_eval() {
  longjmp(eval_jmp_buffer, 666);
}

void eval() {
  if (666 == setjmp(eval_jmp_buffer)) {
    return;
  }
  else {
    cell rs_base = rs_pointer;
    while (rs_pointer >= rs_base) {
      jo* function_body = rs_pop();
      rs_push(function_body + 1);
      cell jo = *(cell*)function_body;
      jo_apply(jo);
    }
  }
}

void eval_jo(jo jo) {
  cell jo_type = jotable[jo].type;
  if (jo_type == str2jo("primitive")) {
    primitive primitive = jotable_get_primitive(jo);
    primitive();
  }
  else if (jo_type == str2jo("function")) {
    jojo jojo = jotable_get_jojo(jo);
    rs_push(jojo.array);
    eval();
  }
  else if (jo_type == str2jo("variable")) {
    cell cell = jotable_get_cell(jo);
    as_push(cell);
  }
}

void k_ignore();

void eval_key(jo jo) {
  if (!jotable_entry_used(jotable[jo])) {
    printf("undefined keyword : %s\n", jo2str(jo));
    k_ignore();
    return;
  }
  eval_jo(jo);
}

void eval_jojo(jo* array) {
  rs_push(array);
  eval();
}

void cell_copy(cell length, cell* from, cell* to) {
  cell i = 0;
  while (i < length) {
    to[i] = from[i];
    i++;
  }
}

void p_drop() {
  // (a ->)
  as_pop();
}

void p_dup() {
  // (a a -> a)
  cell a = as_pop();
  as_push(a);
  as_push(a);
}

void p_over() {
  // (a b -> a b a)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a);
  as_push(b);
  as_push(a);
}

void p_tuck() {
  // (a b -> b a b)
  cell b = as_pop();
  cell a = as_pop();
  as_push(b);
  as_push(a);
  as_push(b);
}

void p_swap() {
  // (a b -> b a)
  cell b = as_pop();
  cell a = as_pop();
  as_push(b);
  as_push(a);
}

void p_xy_swap() {
  // (xxx yyy x y -> yyy xxx)
  cell y = as_pop();
  cell x = as_pop();
  cell* yp = calloc(y, sizeof(cell));
  cell* xp = calloc(x, sizeof(cell));
  cell_copy(y, (as + (as_pointer - y)), yp);
  cell_copy(x, (as + (as_pointer - y - x)), xp);
  cell_copy(y, yp, (as + (as_pointer - y - x)));
  cell_copy(x, xp, (as + (as_pointer - x)));
  free(yp);
  free(xp);
}

void p_print_stack() {
  // ([io] ->)
  printf("\n");
  if (as_pointer < as_base) {
    printf("  * %ld *  ", (as_pointer - as_base));
    printf("-- below the stack --\n");
  }
  else {
    printf("  * %ld *  ", (as_pointer - as_base));
    printf("-- ");
    cell i = as_base;
    while (i < as_pointer) {
      printf("%ld ", as[i]);
      i++;
    }
    printf("--\n");
  }
}

void p_stack_base() {
  as_push(as + as_base);
}

void p_stack_pointer() {
  as_push(as + as_pointer);
}

void export_stack_operation() {
  defprim("drop", p_drop);
  defprim("dup", p_dup);
  defprim("over", p_over);
  defprim("tuck", p_tuck);
  defprim("swap", p_swap);
  defprim("xy-swap", p_xy_swap);
  defprim("print-stack", p_print_stack);
  defprim("stack-pointer", p_stack_pointer);
  defprim("stack-base", p_stack_base);
}

void p_end() {
  // (rs: addr ->)
  rs_pop();
}

void p_bye() {
  // (-> [exit])
  printf("bye bye ^-^/\n");
  exit(0);
}

void export_ending() {
  defprim("end", p_end);
  defprim("bye", p_bye);
}

void i_lit() {
  // ([rs] -> int)
  jo* function_body = rs_pop();
  rs_push(function_body + 1);
  cell jo = *(cell*)function_body;
  as_push(jo);
}

void i_tail_call() {
  // ([rs] -> int)
  jo* function_body = rs_pop();
  cell jo = *(cell*)function_body;
  jo_apply(jo);
}

void p_jump_if_false() {
  // (bool addr -> [rs])
  jo* a = as_pop();
  cell b = as_pop();
  if (b == 0) {
    rs_pop();
    rs_push(a);
  }
}

void export_control() {
  defprim("i-lit", i_lit);
  defprim("i-tail-call", i_tail_call);
  defprim("jump-if-false", p_jump_if_false);
}

void p_true() {
  as_push(1);
}

void p_false() {
  as_push(0);
}

void p_not() {
  // (bool -> bool)
  cell a = as_pop();
  as_push(!a);
}

void p_and() {
  // (bool bool -> bool)
  cell a = as_pop();
  cell b = as_pop();
  as_push(a&&b);
}

void p_or() {
  // (bool bool -> bool)
  cell a = as_pop();
  cell b = as_pop();
  as_push(a||b);
}

void export_bool() {
  defprim("true", p_true);
  defprim("false", p_false);
  defprim("not", p_not);
  defprim("and", p_and);
  defprim("or", p_or);
}

void p_bit_true() {
  // (-> cell)
  cell i = -1;
  as_push(i);
}

void p_bit_false() {
  // (-> cell)
  as_push(0);
}

void p_bit_and() {
  // (cell cell -> cell)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a&b);
}

void p_bit_or() {
  // (cell cell -> cell)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a|b);
}

void p_bit_xor() {
  // (cell cell -> cell)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a^b);
}

void p_bit_not() {
  // (cell -> cell)
  cell a = as_pop();
  as_push(~a);
}

void p_bit_shift_left() {
  // (cell step -> cell)
  cell s = as_pop();
  cell a = as_pop();
  as_push(a<<s);
}

void export_bit() {
  defprim("bit/true", p_bit_true);
  defprim("bit/false", p_bit_false);
  defprim("bit/not", p_bit_not);
  defprim("bit/and", p_bit_and);
  defprim("bit/xor", p_bit_xor);
  defprim("bit/or", p_bit_or);
  defprim("bit/shift-left", p_bit_shift_left);
  // defprim("bit/shift-right", p_bit_shift_right);
  // defprim("bit/arithmetic-shift-right", p_bit_arithmetic_shift_right);
}

void p_add() {
  // (cell cell -> int)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a + b);
}

void p_sub() {
  // (cell cell -> int)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a - b);
}

void p_mul() {
  // (cell cell -> int)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a * b);
}

void p_div() {
  // (cell cell -> int)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a / b);
}

void p_mod() {
  // (cell cell -> int)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a % b);
}

void p_n_eq_p() {
  // (a ... b ... n -> bool)
  cell n = as_pop();
  cell old_n = n;
  cell* cursor1 = (as + as_pointer - n);
  cell* cursor2 = (as + as_pointer - n - n);
  while (n > 0) {
    if (cursor1[n-1] != cursor2[n-1]) {
      as_pointer = as_pointer - old_n - old_n;
      as_push(false);
      return;
    }
    n--;
  }
  as_pointer = as_pointer - old_n - old_n;
  as_push(true);
}

void p_eq_p() {
  // (cell cell -> bool)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a == b);
}

void p_gt_p() {
  // (cell cell -> bool)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a > b);
}

void p_lt_p() {
  // (cell cell -> bool)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a < b);
}

void p_gteq_p() {
  // (cell cell -> bool)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a >= b);
}

void p_lteq_p() {
  // (cell cell -> bool)
  cell b = as_pop();
  cell a = as_pop();
  as_push(a <= b);
}

jo read_jo();

void k_int() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      break;
    }
    else if (int_string_p(jo2str(s))) {
      here(str2jo("i-lit"));
      here(string_to_dec(jo2str(s)));
    }
    else {
      printf("meet non-cell string in (int ...) : %s", jo2str(s));
      break;
    }
  }
}

void k_bin() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      break;
    }
    else if (int_string_p(jo2str(s))) {
      here(str2jo("i-lit"));
      here(string_to_bin(jo2str(s)));
    }
    else {
      printf("meet non-cell string in (int ...) : %s", jo2str(s));
      break;
    }
  }
}

void k_oct() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      break;
    }
    else if (int_string_p(jo2str(s))) {
      here(str2jo("i-lit"));
      here(string_to_oct(jo2str(s)));
    }
    else {
      printf("meet non-cell string in (int ...) : %s", jo2str(s));
      break;
    }
  }
}

void k_hex() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      break;
    }
    else if (int_string_p(jo2str(s))) {
      here(str2jo("i-lit"));
      here(string_to_hex(jo2str(s)));
    }
    else {
      printf("meet non-cell string in (int ...) : %s", jo2str(s));
      break;
    }
  }
}

void p_print_int() {
  // (cell -> [io])
  printf("%ld", as_pop());
}

void p_dot() {
  // (cell -> [io])
  printf("%ld ", as_pop());
}

void export_int() {
  defprim("add", p_add);
  defprim("sub", p_sub);

  defprim("mul", p_mul);
  defprim("div", p_div);
  defprim("mod", p_mod);

  defprim("neg", p_not);

  defprim("n-eq?", p_n_eq_p);

  defprim("eq?", p_eq_p);
  defprim("gt?", p_gt_p);
  defprim("lt?", p_lt_p);
  defprim("gteq?", p_gteq_p);
  defprim("lteq?", p_lteq_p);

  defprim("int", k_int);
  defprim("dec", k_int);
  defprim("bin", k_bin);
  defprim("oct", k_oct);
  defprim("hex", k_hex);

  defprim("print-int", p_print_int);
  defprim("dot", p_dot);
}

void p_allocate () {
  // (size -> addr)
  as_push(calloc(as_pop(), 1));
}

void p_free () {
  // (addr ->)
  free(as_pop());
}

void k_var() {
  // ([io] -> [compile])
  here(str2jo("i-lit"));
  jo index = read_jo();
  here(&(jotable[index].value.cell));
  k_ignore();
}

void p_set() {
  // (cell addr ->)
  cell* address = as_pop();
  cell value = as_pop();
  address[0] = value;
}

void p_get() {
  // (addr -> cell)
  cell* address = as_pop();
  as_push(address[0]);
}

void p_set_byte() {
  // (cell addr ->)
  char* address = as_pop();
  cell value = as_pop();
  address[0] = value;
}

void p_get_byte() {
  // (addr -> cell)
  char* address = as_pop();
  as_push(address[0]);
}

void export_memory() {
  defprim("allocate", p_allocate);
  defprim("free", p_free);
  defprim("var", k_var);
  defprim("set", p_set);
  defprim("get", p_get);
  defprim("set-byte", p_set_byte);
  defprim("get-byte", p_get_byte);
}

typedef FILE* reading_stack_t[64];

reading_stack_t reading_stack;
cell reading_stack_base = 0;
cell reading_stack_pointer = 0;

void reading_stack_push(FILE* value) {
  reading_stack[reading_stack_pointer] = value;
  reading_stack_pointer++;
}

FILE* reading_stack_pop() {
  reading_stack_pointer--;
  return reading_stack[reading_stack_pointer];
}

FILE* reading_stack_tos() {
  return reading_stack[reading_stack_pointer - 1];
}

bool reading_stack_empty_p() {
  return reading_stack_pointer == reading_stack_base;
}

char read_char() {
  if (reading_stack_empty_p()) {
    return fgetc(stdin);
  }
  else {
    char c = fgetc(reading_stack_tos());
    if (c == EOF) {
      fclose(reading_stack_pop());
      return read_char();
    }
    else {
      return c;
    }
  }
}

void unread_char(char c) {
  if (reading_stack_empty_p()) {
    ungetc(c, stdin);
  }
  else {
    ungetc(c, reading_stack_tos());
  }
}

typedef struct {
  jo nick;
  jo name;
} alias;

typedef alias* loading_stack_t[64];

cell alias_record_size = 1024;

alias loading_stack_area[64][1024];

loading_stack_t loading_stack;
cell loading_stack_base = 0;
cell loading_stack_pointer = 0;

void loading_stack_push(alias* value) {
  loading_stack[loading_stack_pointer] = value;
  loading_stack_pointer++;
}

alias* loading_stack_pop() {
  loading_stack_pointer--;
  return loading_stack[loading_stack_pointer];
}

alias* loading_stack_tos() {
  return loading_stack[loading_stack_pointer - 1];
}

bool loading_stack_empty_p() {
  return loading_stack_pointer == loading_stack_base;
}

void init_loading_stack() {
  alias record[alias_record_size];
  alias a = {.nick = 0, .name = 0};
  record[0] = a;
  loading_stack_push(record);
}

void alias_add(jo nick, jo name) {
  alias* alias_record = loading_stack_tos();
  cell i = 0;
  while (i < alias_record_size) {
    if (alias_record[i].nick == 0 &&
        alias_record[i].name == 0) {
      alias_record[i].nick = nick;
      alias_record[i].name = name;
      alias_record[i+1].nick = 0;
      alias_record[i+1].name = 0;
      return;
    }
    else {
      i++;
    }
  }
  printf("alias_add fail alias_record is full\n");
}

jo alias_find(jo nick) {
  // return 0 -- not found
  alias* alias_record = loading_stack_tos();
  cell i = 0;
  while (true) {
    if (alias_record[i].nick == 0 &&
        alias_record[i].name == 0) {
      return 0;
    }
    else if (alias_record[i].nick == nick) {
      return alias_record[i].name;
    }
    else {
      i++;
    }
  }
}

jo read_jo_without_prefix() {
  // ([io] -> jo)
  char buf[1024];
  cell cur = 0;
  cell collecting = false;
  char c;
  char go = true;
  while (go) {
    c = read_char();
    if (!collecting) {
      if (isspace(c)) {
        // do nothing
      }
      else {
        collecting = true;
        buf[cur] = c;
        cur++;
        if (isbarcket(c)) {
          go = false;
        }
      }
    }
    else {
      if (isbarcket(c) ||
          isspace(c)) {
        unread_char(c);
        go = false;
      }
      else {
        buf[cur] = c;
        cur++;
      }
    }
  }
  buf[cur] = 0;
  return str2jo(buf);
}

jo read_jo() {
  // ([io] -> jo)
  jo jo0 = read_jo_without_prefix();
  jo jo1 = alias_find(jo0);
  if (jo1 != 0) {
    return jo1;
  }
  else {
    return jo0;
  }
}

void p_read_jo_without_prefix() {
  as_push(read_jo_without_prefix());
}

void p_read_jo() {
  as_push(read_jo());
}

jo cat_2_jo(jo x, jo y) {
  char str[2 * 1024];
  str[0] = 0;
  strcat(str, jo2str(x));
  strcat(str, jo2str(y));
  return str2jo(str);
}

jo cat_3_jo(jo x, jo y, jo z) {
  char str[3 * 1024];
  str[0] = 0;
  strcat(str, jo2str(x));
  strcat(str, jo2str(y));
  strcat(str, jo2str(z));
  return str2jo(str);
}

void p_jo_used_p() {
  // (jo -> bool)
  jo jo = as_pop();
  as_push(jotable_entry_used(jotable[jo]));
}

void p_jo_to_string() {
  // (jo -> string)
  jo jo = as_pop();
  as_push(jo2str(jo));
}

void p_string_length_to_jo() {
  // (string length -> jo)
  cell len = as_pop();
  cell str = as_pop();
  char buffer[2 * 1024];
  strncpy(buffer, str, len);
  buffer[len] = 0;
  as_push(str2jo(buffer));
}

void p_string_to_jo() {
  // (string -> jo)
  string str = as_pop();
  as_push(str2jo(str));
}

void p_null() {
  as_push(str2jo("null"));
}

void k_jo() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo("(")) {
      eval_key(read_jo());
    }
    else if (s == str2jo(")")) {
      break;
    }
    else {
      here(str2jo("i-lit"));
      here(s);
    }
  }
}

void export_jo() {
  defprim("null", p_null);
  defprim("read-jo", p_read_jo);
  defprim("read-jo-without-prefix", p_read_jo_without_prefix);
  defprim("jo-used?", p_jo_used_p);
  defprim("jo->string", p_jo_to_string);
  defprim("string->jo", p_string_to_jo);
  defprim("string/length->jo", p_string_length_to_jo);
  defprim("jo", k_jo);
}

void k_one_string() {
  // ([io] -> [compile])
  char buffer[1024 * 1024];
  cell cursor = 0;
  while (true) {
    char c = read_char();
    if (c == '"') {
      buffer[cursor] = 0;
      cursor++;
      break;
    }
    else {
      buffer[cursor] = c;
      cursor++;
    }
  }
  string str = malloc(cursor);
  strcpy(str, buffer);
  here(str2jo("i-lit"));
  here(str);
}

void k_string() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      return;
    }
    else if (s == str2jo("\"")) {
      k_one_string();
    }
    else {
      // do nothing
    }
  }
}

void p_print_string() {
  // (string -> [io])
  printf("%s", as_pop());
}

void export_string() {
  defprim("string", k_string);
  defprim("print-string", p_print_string);
}

void p_read_file() {
  // (string addr number -> number)
  cell limit = as_pop();
  cell buffer = as_pop();
  cell path = as_pop();
  FILE* fp = fopen(path, "r");
  if(!fp) {
    perror("p_read_file file to open file");
    return;
  }
  cell readed_counter = fread(buffer, 1, limit, fp);
  fclose(fp);
  as_push(readed_counter);
}

bool file_readable_p(string path) {
  FILE* fp = fopen(path, "r");
  if (!fp) {
    return false;
  }
  else {
    fclose(fp);
    return true;
  }
}

bool dir_ok_p(string path) {
  DIR* dir = opendir(path);
  if (!dir) {
    return false;
  }
  else {
    closedir(dir);
    return true;
  }
}

void export_file() {
  defprim("read-file", p_read_file);
}

void p_getcwd() {
  // (-> string)
  char buf[1024];
  as_push(getcwd(buf, 1024));
}

void export_system() {
  defprim("getcwd", p_getcwd);
}

typedef struct {
  jo name;
  jo dir;
  jo* export;
} module;

typedef module module_record_t[1024];

module_record_t module_record;
cell module_record_base = 0;
cell module_record_pointer = 0;

void module_record_push(module value) {
  module_record[module_record_pointer] = value;
  module_record_pointer++;
}

bool module_record_empty_p() {
  return module_record_pointer == module_record_base;
}

bool module_record_find(jo name) {
  cell i = 0;
  while (i < module_record_pointer) {
    if (name == module_record[i].name) {
      return true;
    }
    i++;
  }
  return false;
}

jo* module_record_get_export(jo name) {
  // 0 -- not found
  cell i = module_record_base;
  while (i < module_record_pointer) {
    if (name == module_record[i].name) {
      return module_record[i].export;
    }
    i++;
  }
  return false;
}

void module_record_set_export(jo name, jo* export) {
  cell i = module_record_base;
  while (i < module_record_pointer) {
    if (name == module_record[i].name) {
      module_record[i].export = export;
      return;
    }
    i++;
  }
  printf("module_record_set_export fail\n",
         "can not find module: %s\n", jo2str(name));
}

typedef module module_stack_t[1024];

module_stack_t module_stack;
cell module_stack_base = 0;
cell module_stack_pointer = 0;

void module_stack_push(module value) {
  module_stack[module_stack_pointer] = value;
  module_stack_pointer++;
}

bool module_stack_empty_p() {
  return module_stack_pointer == module_stack_base;
}

module module_stack_pop() {
  module_stack_pointer--;
  return module_stack[module_stack_pointer];
}

module module_stack_tos() {
  return module_stack[module_stack_pointer - 1];
}

void load_file(string path) {
  // [reading_stack]
  FILE* fp = fopen(path, "r");
  if(!fp) {
    perror("File opening failed");
    printf("load_file fail : %s\n", path);
    return;
  }
  reading_stack_push(fp);
}

void k_one_include() {
  // ([io] -> *)
  char buffer[1024 * 1024];
  cell cursor = 0;
  while (true) {
    char c = read_char();
    if (c == '"') {
      buffer[cursor] = 0;
      cursor++;
      break;
    }
    else {
      buffer[cursor] = c;
      cursor++;
    }
  }
  load_file(buffer);
}

void k_include() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      return;
    }
    else if (s == str2jo("(")) {
      eval_key(read_jo());
    }
    else if (s == str2jo("\"")) {
      k_one_include();
    }
    else {
      // do nothing
    }
  }
}

string user_module_dir = "/.jojo/module/";
string system_module_dir = "";

jo get_module_path(jo name) {
  // return 0 -- not found
  char path[4 * 1024];
  path[0] = 0;
  strcat(path, getenv("HOME"));
  strcat(path, user_module_dir);
  strcat(path, jo2str(name));
  strcat(path, "/");
  strcat(path, "module.jo");
  if (file_readable_p(path)) {
    return str2jo(path);
  }
  else {
    return 0;
  }
}

jo get_module_dir(jo name) {
  // return 0 -- not found
  char path[4 * 1024];
  path[0] = 0;
  strcat(path, getenv("HOME"));
  strcat(path, user_module_dir);
  strcat(path, jo2str(name));
  strcat(path, "/");
  if (dir_ok_p(path)) {
    return str2jo(path);
  }
  else {
    return 0;
  }
}

void import_module(jo name) {
  jo* export = module_record_get_export(name);
  if (export == 0) {
    printf("import_module fail to import: %s\n", jo2str(name));
    return;
  }
  cell i = 0;
  while (export[i] != 0) {
    jo new_jo = cat_3_jo(name,
                         str2jo("/"),
                         export[i]);
    alias_add(export[i], new_jo);
    i++;
  }
}

bool k_dep_load(jo name) {
  jo module_path = get_module_path(name);
  jo module_dir = get_module_dir(name);
  if (module_path == 0) {
    return false;
  }

  jo export[1];
  export[0] = 0;
  module m = {
    .name = name,
    .dir = module_dir,
    .export = export
  };
  module_record_push(m);
  module_stack_push(m);

  alias a = {.nick = 0, .name = 0};
  loading_stack_area[loading_stack_pointer][0] = a;
  loading_stack_push(loading_stack_area[loading_stack_pointer]);

  load_file(jo2str(module_path));

  return true;
}

void k_dep() {
  // ([io] -> [loading_stack])
  jo prefix = read_jo_without_prefix();
  jo version = read_jo_without_prefix();
  jo name = cat_3_jo(prefix, str2jo("/"), version);
  if (!module_record_find(name)) {
    bool result = k_dep_load(name);
    if (result == false) {
      printf("k_dep fail to load module : %s\n", jo2str(name));
      k_ignore();
    }
    else {
      while (true) {
        jo s = read_jo();
        if (s == str2jo("(")) {
          eval_key(read_jo());
        }
        else if (s == str2jo(")")) {
          loading_stack_pop();
          module_stack_pop();
          import_module(name);
          break;
        }
        else {
          // do nothing
        }
      }
    }
  }
}

void k_module() {
  // ([io] -> [loading_stack_tos])
  jo prefix = read_jo_without_prefix();
  jo version = read_jo_without_prefix();
  jo name = cat_3_jo(prefix, str2jo("/"), version);
  // ><><>< check module name

  jo* export = compiling_stack_tos();
  while (true) {
    jo s = read_jo_without_prefix();
    if (s == str2jo(")")) {
      here(0);
      module_record_set_export(name, export);
      return;
    }
    else if (!alias_find(s) == 0) {
      printf("k_module fail, alias used : %s\n", jo2str(s));
      k_ignore();
      return;
    }
    else {
      here(s);
    }
  }
}

void one_module_report(module m) {
  printf("  - %s -- %s\n", jo2str(m.name), jo2str(m.dir));
  cell i = 0;
  while (m.export[i] != 0) {
    printf("    %s\n", jo2str(m.export[i]));
    i++;
  }
}

void module_report() {
  printf("- module_report\n");
  cell i = module_record_base;
  while (i < module_record_pointer) {
    one_module_report(module_record[i]);
    i++;
  }
}

void export_module() {
  defprim("include", k_include);
  defprim("dep", k_dep);
  defprim("module", k_module);
  defprim("module-report", module_report);
}

void* get_clib(string rel_path) {
  char path[2 * 1024];
  path[0] = 0;
  strcat(path, jo2str(module_stack_tos().dir));
  strcat(path, rel_path);
  void* lib = dlopen(path, RTLD_LAZY);
  if (lib == NULL) {
    printf("fail to open library : %s : %s\n",
           path, dlerror());
  };
  return lib;
}

void ccall (string str, void* lib) {
  primitive fun = dlsym(lib, str);
  if (fun == NULL) {
    printf("can not find %s function lib : %s\n",
           str, dlerror());
  };
  fun();
}

void k_one_clib() {
  // ([io] -> [compile])
  char buffer[1024];
  cell cursor = 0;
  while (true) {
    char c = read_char();
    if (c == '"') {
      buffer[cursor] = 0;
      cursor++;
      break;
    }
    else {
      buffer[cursor] = c;
      cursor++;
    }
  }
  ccall("export", get_clib(buffer));
}

void k_clib() {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      return;
    }
    else if (s == str2jo("\"")) {
      k_one_clib();
    }
    else {
      // do nothing
    }
  }
}

void export_ffi() {
  defprim("clib", k_clib);
}

bool prim_jo_p(jo index) {
  return jotable[index].type == str2jo("primitive");
}

bool fun_jo_p(jo index) {
  return jotable[index].type == str2jo("function");
}

bool var_jo_p(jo index) {
  return jotable[index].type == str2jo("variable");
}

bool used_jo_p(jo index) {
  return
    prim_jo_p(index) ||
    fun_jo_p(index) ||
    var_jo_p(index);
}

jo jo_to_jo_in_module(jo alias_jo) {
  if (module_stack_empty_p()) {
    return alias_jo;
  }
  else {
    jo new_jo = cat_3_jo(module_stack_tos().name,
                         str2jo("/"),
                         alias_jo);
    alias_add(alias_jo, new_jo);
    return new_jo;
  }
}

jo read_jo_in_module() {
  jo_to_jo_in_module(read_jo());
}

jo defun_record[64 * 1024];
cell defun_record_counter = 0;

void p_defun_record() {
  // (-> addr)
  as_push(defun_record);
}

void defun_report() {
  printf("- defun_report // counter : %ld\n", defun_record_counter);
  cell i = 0;
  while (i < defun_record_counter) {
    printf("  %s\n", jo2str(defun_record[i]));
    i++;
  }
  printf("\n");
}

typedef jo defun_stack_t[1024];

defun_stack_t defun_stack;
cell defun_stack_base = 0;
cell defun_stack_pointer = 0;

void defun_stack_push(jo* value) {
  defun_stack[defun_stack_pointer] = value;
  defun_stack_pointer++;
}

jo* defun_stack_pop() {
  defun_stack_pointer--;
  return defun_stack[defun_stack_pointer];
}

void defun_stack_inc() {
  defun_stack[defun_stack_pointer - 1] =
    defun_stack[defun_stack_pointer - 1] + 1;
}


jo* defun_stack_tos() {
  return defun_stack[defun_stack_pointer - 1];
}

bool defun_stack_empty_p() {
  return defun_stack_pointer == defun_stack_base;
}

void k_compile_jojo();

void k_defun() {
  // ([io] -> [compile] [jotable])
  jo index = read_jo_in_module();
  if (used_jo_p(index)) {
    printf("- defun can not re-define : %s\n", jo2str(index));
    printf("  it already defined as : %s\n", jo2str(jotable[index].type));
    k_ignore();
    return;
  }
  defun_stack_push(index);
  defun_record[defun_record_counter] = index;
  defun_record_counter++;
  defun_record[defun_record_counter] = 0;
  jo* array = compiling_stack_tos();
  k_compile_jojo();
  here(str2jo("end"));
  jotable[index].type = str2jo("function");
  jotable[index].value.jojo.size = compiling_stack_tos() - array;
  jotable[index].value.jojo.array = array;
  defun_stack_pop();
}

void k_one_declare() {
  jo index = read_jo_in_module();
  jotable[index].type = str2jo("declared");
  k_ignore();
}

void k_declare() {
  while (true) {
    jo s = read_jo();
    if (s == str2jo(")")) {
      return;
    }
    else if (s == str2jo("(")) {
      k_one_declare();
    }
    else {
      // do nothing
    }
  }
}

void k_run() {
  // ([io] -> *)
  jo array[64 * 1024];
  compiling_stack_push(array);
  while (true) {
    jo s = read_jo();
    if (s == str2jo("(")) {
      eval_key(read_jo());
    }
    else if (s == str2jo(")")) {
      here(str2jo("end"));
      break;
    }
    else {
      here(s);
    }
  }
  compiling_stack_pop();
  eval_jojo(array);
}

jo defvar_record[64 * 1024];
cell defvar_record_counter = 0;

void p_defvar_record() {
  // (-> addr)
  as_push(defvar_record);
}

void defvar_report() {
  printf("- defvar_report // counter : %ld\n", defvar_record_counter);
  cell i = 0;
  while (i < defvar_record_counter) {
    printf("  %s\n", jo2str(defvar_record[i]));
    i++;
  }
  printf("\n");
}

void k_defvar() {
  // ([io] -> [compile] [jotable])
  jo index = read_jo_in_module();
  if (used_jo_p(index)) {
    printf("- defvar can not re-define : %s\n", jo2str(index));
    printf("  it already defined as : %s\n", jo2str(jotable[index].type));
    k_ignore();
    return;
  }
  defvar_record[defvar_record_counter] = index;
  defvar_record_counter++;
  defvar_record[defvar_record_counter] = 0;
  k_run();
  jotable_set_cell(index, as_pop());
}

void p_top_repl() {
  // ([io] -> *)
  while (true) {
    jo s = read_jo();
    if (s == str2jo("(")) {
      eval_key(read_jo());
    }
    else {
      // do nothing
    }
  }
}

void export_top_level() {
  defprim("defun-record", p_defun_record);
  defprim("defun-report", defun_report);

  defprim("defun", k_defun);

  defprim("declare", k_declare);

  defprim("run", k_run);

  defprim("defvar-record", p_defvar_record);
  defprim("defvar-report", defvar_report);
  defprim("defvar", k_defvar);

  defprim("top-repl", p_top_repl);
}

void k_ignore() {
  // ([io] ->)
  while (true) {
    jo s = read_jo();
    if (s == str2jo("(")) {
      k_ignore();
    }
    if (s == str2jo(")")) {
      break;
    }
  }
}

void compile_jojo_until_meet_jo(jo end) {
  // ([io] -> [compile])
  while (true) {
    jo s = read_jo();
    if (s == str2jo("(")) {
      eval_key(read_jo());
    }
    else if (s == end) {
      break;
    }
    else if (jotable_entry_used(jotable[s]) ||
             defun_stack_empty_p() ||
             defun_stack_tos() == s) {
      here(s);
    }
    else {
      // no compile before define
      printf("- k_compile_jojo undefined : %s\n", jo2str(s));
      k_ignore();
      return;
    }
  }
}

void k_compile_jojo_until_meet_jo() {
  compile_jojo_until_meet_jo(as_pop());
}

void k_compile_jojo() {
  // ([io] -> [compile])
  compile_jojo_until_meet_jo(str2jo(")"));
}

void k_if() {
  // ([io] -> [compile])
  compile_jojo_until_meet_jo(str2jo("->"));
  here(str2jo("i-lit"));
  cell* offset_place = compiling_stack_tos();
  compiling_stack_inc();
  here(str2jo("jump-if-false"));
  k_compile_jojo();
  offset_place[0] = compiling_stack_tos();
}

void k_tail_call() {
  // ([io] -> [compile])
  // no check for "no compile before define"
  here(str2jo("i-tail-call"));
  jo s = read_jo();
  here(s);
  k_ignore();
}

void k_loop() {
  here(str2jo("i-tail-call"));
  here(defun_stack_tos());
  k_ignore();
}

void k_recur() {
 here(defun_stack_tos());
 k_ignore();
}

void p_compiling_stack_tos() {
  as_push(compiling_stack_tos());
}

void i_jojo() {
  // ([rs] -> int)
  jo* function_body = rs_pop();
  rs_push(function_body[0]);
  as_push(function_body + 1);
}

void k_jojo() {
  // ([io] -> [compile])
  here(str2jo("i-jojo"));
  cell* offset_place = compiling_stack_tos();
  compiling_stack_inc();
  k_compile_jojo();
  here(str2jo("end"));
  offset_place[0] = compiling_stack_tos();
}

void export_keyword() {
  defprim(":", k_ignore);
  defprim("note", k_ignore);

  defprim("compiling-stack-tos", p_compiling_stack_tos);
  defprim("compiling-stack-inc", compiling_stack_inc);

  defprim("if", k_if);
  defprim("do", k_compile_jojo);
  defprim("compile-jojo-until-meet-jo", k_compile_jojo_until_meet_jo);
  defprim("else", k_compile_jojo);
  defprim("tail-call", k_tail_call);
  defprim("loop", k_loop);
  defprim("recur", k_recur);

  defprim("jojo", k_jojo);
  defprim("i-jojo", i_jojo);
}

void do_nothing() {
}

void p_here() {
  here(as_pop());
}

void p_round_bar() { as_push(str2jo("(")); }
void p_round_ket() { as_push(str2jo(")")); }
void p_square_bar() { as_push(str2jo("[")); }
void p_square_ket() { as_push(str2jo("]")); }
void p_flower_bar() { as_push(str2jo("{")); }
void p_flower_ket() { as_push(str2jo("}")); }
void p_double_quote() { as_push(str2jo("\"")); }

void p_cell_size() {
  // (-> cell)
  as_push(sizeof(cell));
}

void p_newline() {
  printf("\n");
}

void export_mise() {
  defprim("here", p_here);
  defprim("jo/apply", p_jo_apply);
  defprim("apply", p_apply);
  defprim("jotable-report", jotable_report);

  defprim("round-bar", p_round_bar);
  defprim("round-ket", p_round_ket);
  defprim("square-bar", p_square_bar);
  defprim("square-ket", p_square_ket);
  defprim("flower-bar", p_flower_bar);
  defprim("flower-ket", p_flower_ket);
  defprim("double-quote", p_double_quote);

  defprim("cell-size", p_cell_size);
  defprim("defprim-report", defprim_report);
  defprim("newline", p_newline);
}

void p1() {
  printf("- p1\n");
  printf("  %ld %ld %ld\n", sizeof(void*), sizeof(cell), sizeof(unsigned));
  printf("  %ld %ld\n", sizeof((cell)-1), sizeof(-1));
  printf("  %x %x\n", 1<<sizeof(cell), 32>>6);
  printf("  %x %x %x\n", -1>>2, (cell)-1>>2, (unsigned)-1>>2);
  printf("  %ld %ld\n", string_to_bin("1000"), string_to_hex("ffff"));
  printf("  %ld %ld %ld %ld\n", '0', '1', 'A', 'a');
}

void p2() {
  printf("- p2\n");
}

cell string_to_sum_test(string str) {
  cell sum = 0;
  cell max_step = 10;
  cell i = 0;
  while (i < strlen(str)) {
    sum = sum + ((unsigned char) str[i]) * (2 << min(i, max_step));
    printf("| char: %ld | unsigned char: %ld | sum: %ld |\n", str[i], (unsigned char) str[i], sum);
    i++;
  }
  return sum;
}

void p3() {
  printf("- p3\n");
  printf("  %ld\n", string_to_sum_test("abcabcabc"));
  printf("  %ld\n", string_to_sum_test("中中"));
  printf("  %ld\n", string_to_sum_test("中中中"));
}

void export_play() {
  defprim("p1", p1);
  defprim("p2", p2);
  defprim("p3", p3);
}

void init_top_repl() {
  init_jotable();
  init_compiling_stack();
  init_loading_stack();

  export_stack_operation();
  export_ending();
  export_control();
  export_bool();
  export_bit();
  export_int();
  export_memory();
  export_jo();
  export_string();
  export_file();
  export_keyword();
  export_system();
  export_module();
  export_ffi();
  export_top_level();
  export_mise();
  export_play();
}

int main(int argc, string* argv) {
  init_top_repl();
  if (argc != 1) {
    load_file(argv[1]);
  }
  p_top_repl();
}
