#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>

typedef enum { false, true } BOOL;

typedef char* STRING;

BOOL
string_equal
(STRING string1, STRING string2) {
  if (strcmp(string1, string2) == 0) {
    return true;
  }
  else {
    return false;
  }
}

typedef uint8_t BYTE;
typedef uint32_t CELL;
typedef CELL ADDR;

#define CELL_SIZE 4;
CELL cell_size = 4;

CELL memory_size = 1024 * 1024;
BYTE memory[1024 * 1024];
ADDR memory_cursor = 0;

BYTE
memory_get_byte
(CELL index) {
  return (BYTE) memory[index];
}

void
memory_set_byte
(CELL index, BYTE value) {
  memory[index] = value;
}

CELL
memory_get
(CELL index) {
    CELL sum = 0;
    CELL i;
    CELL base = 1;
    for (i=0; i < cell_size; i=i+1) {
      sum = sum + (memory_get_byte(index + i) * base);
      base = base * 256;
    }
    return sum;
}

void
memory_set
(CELL index, CELL value) {
  CELL i;
  for (i=0; i < cell_size; i=i+1) {
    memory_set_byte(index + i, value % 256);
    value = value >> 8;
  }
}

CELL
memory_allocate
(CELL size) {
  CELL return_address = memory_cursor;
  memory_cursor = return_address + size;
  return return_address;
}

typedef CELL STACK[1024 * 4];

STACK as;
CELL as_pointer = 0;

void
as_push
(CELL value) {
  as[as_pointer] = value;
  as_pointer = as_pointer + cell_size;
}

CELL
as_pop
() {
  as_pointer = as_pointer - cell_size;
  return as[as_pointer];
}

STACK rs;
CELL rs_pointer = 0;

void
rs_push
(CELL value) {
  rs[rs_pointer] = value;
  rs_pointer = rs_pointer + cell_size;
}

CELL
rs_pop
() {
  rs_pointer = rs_pointer - cell_size;
  return rs[rs_pointer];
}

typedef void (*PRIMITIVE)();

typedef PRIMITIVE PRIMITIVE_RECORD[1024];

PRIMITIVE_RECORD primitive_record;
CELL primitive_record_counter = 0;

PRIMITIVE
primitive_record_get
(CELL index) {
  return primitive_record[index];
}

void
primitive_record_set
(CELL index, PRIMITIVE fun) {
  primitive_record[index] = fun;
}

CELL
create_primitive
(PRIMITIVE fun) {
  CELL return_address = primitive_record_counter;
  primitive_record_set
    (primitive_record_counter, fun);
  primitive_record_counter =
    primitive_record_counter + 1;
  return return_address;
}

CELL address_after_explainer = 0;

jmp_buf jmp_buffer;

BOOL
exit_interpreter
() {
  longjmp(jmp_buffer, 666);
}

void
interpreter
() {
  if (666 == setjmp(jmp_buffer)) {
    return;
  }
  else {
    while (true) {
      ADDR function_body = rs_pop();
      ADDR explainer = memory_get(memory_get(function_body));
      rs_push(function_body + cell_size);
      address_after_explainer =
        memory_get(function_body) + cell_size;
      PRIMITIVE explainer_function =
        primitive_record_get(explainer);
      explainer_function();
    }
  }
}

typedef struct {
  STRING string;
  ADDR address;
} InHostTagRecordEntry;

typedef InHostTagRecordEntry InHostTagRecord[1024];

InHostTagRecord in_host_tag_record;
CELL in_host_tag_record_counter = 0;

ADDR
in_host_tag_record_get
(STRING string) {
  CELL i;
  BOOL match_p;
  for (i=0; i < in_host_tag_record_counter; i=i+1) {
    match_p = (string_equal
               (string,
                (in_host_tag_record[i].string)));
    if (match_p) {
      return (in_host_tag_record[i].address);
    }
  }
}

BYTE in_host_tag_record_string_buffer[1024 * 1024];
CELL in_host_tag_record_string_buffer_counter = 0;

void
in_host_tag_record_set
(STRING string, ADDR address) {
  strcpy((in_host_tag_record_string_buffer +
          in_host_tag_record_string_buffer_counter),
         string);
  in_host_tag_record
    [in_host_tag_record_counter]
    .string = (in_host_tag_record_string_buffer +
               in_host_tag_record_string_buffer_counter);
  in_host_tag_record_string_buffer_counter =
    in_host_tag_record_string_buffer_counter +
    strlen(string) + 1;
  in_host_tag_record
    [in_host_tag_record_counter]
    .address = address;
  in_host_tag_record_counter =
    in_host_tag_record_counter + 1;
}

void
data
(CELL value) {
  memory_set(memory_cursor, value);
  memory_cursor =
    memory_cursor + cell_size;
}

void
mark
(STRING tag_string) {
  in_host_tag_record_set
    (tag_string,
     memory_cursor);
}

CELL link = 0;

void
p_primitive_explainer
() {
  PRIMITIVE primitive =
    (primitive_record_get
     (memory_get
      (address_after_explainer)));
  primitive();
}

CELL primitive_explainer = 0;

void
define_primitive
(STRING tag_string, PRIMITIVE fun) {
  CELL function_index = create_primitive(fun);
  data(link);
  link = memory_cursor - cell_size;
  mark(tag_string);
  data(primitive_explainer);
  data(function_index);
}

void
p_function_explainer
() {
  rs_push(address_after_explainer);
}

CELL function_explainer = 1;

void
define_function
(STRING tag_string, CELL length,
 STRING *function_tag_string_array) {
  data(link);
  link = memory_cursor - cell_size;
  mark(tag_string);
  data(function_explainer);
  CELL i;
  for (i=0; i < length; i=i+1) {
    data(in_host_tag_record_get
         (function_tag_string_array[i]));
  }
}

void
p_variable_explainer
() {
  as_push
    (memory_get(address_after_explainer));
}

CELL variable_explainer = 2;

void
define_variable
(STRING tag_string, CELL value) {
  data(link);
  link = memory_cursor - cell_size;
  mark(tag_string);
  data(variable_explainer);
  data(value);
}

void
p_end
() {
  // (rs: addr ->)
  rs_pop();
}

void
p_bye
() {
  // (-> [exit])
  printf("bye bye ^-^/\n");
  exit_interpreter();
}

void
p_dup
() {
  // (cell cell -> cell)
  CELL a = as_pop();
  as_push(a);
  as_push(a);
}

void
p_mul
() {
  // (integer integer -> integer)
  CELL a = as_pop();
  CELL b = as_pop();
  as_push(a * b);
}

void
p_simple_wirte
() {
  // (integer -> [IO])
  printf("%d\n", as_pop());
}

void
vm1
() {

  create_primitive(p_primitive_explainer);
  create_primitive(p_function_explainer);
  create_primitive(p_variable_explainer);

  define_primitive("end", p_end);
  define_primitive("bye", p_bye);
  define_primitive("dup", p_dup);
  define_primitive("mul", p_mul);
  define_primitive("simple-wirte", p_simple_wirte);

  define_variable("little-test-number", 4);

  STRING p_square[] = {
    "dup",
    "mul",
    "end"
  };
  define_function("square", 3, p_square);

  STRING p_little_test[] = {
    "little-test-number",
    "square",
    "simple-wirte",
    "bye"
  };
  define_function ("little-test", 4, p_little_test);

  STRING p_first_function[] = {
    "little-test",
    "end"
  };
  define_function("first-function", 2, p_first_function);

  rs_push
    (in_host_tag_record_get("first-function") + cell_size);
  interpreter();

}

int
main
(int argc, STRING* argv) {
  vm1();
  return 0;
}
