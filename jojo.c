  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>
  #include <fcntl.h>
  #include <ctype.h>
  #include <stdint.h>
  #include <dlfcn.h>
  #include <dirent.h>
  #include <signal.h>
  #include <limits.h>
  #include <stdarg.h>
  #include <sys/socket.h>
  #include <netdb.h>
  #include <arpa/inet.h>
  typedef enum { false, true } bool;
  typedef intptr_t cell;
  typedef void (* primitive_t)();
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
      bool char_space_p(char c) {
        return isspace(c);
      }
      bool char_bar_ket_p(char c) {
        return (c == '(' ||
                c == ')' ||
                c == '[' ||
                c == ']' ||
                c == '{' ||
                c == '}');
      }
      bool char_delimiter_p(char c) {
        return (c == '(' ||
                c == ')' ||
                c == '[' ||
                c == ']' ||
                c == '{' ||
                c == '}' ||
                c == '"' ||
                c == ',' ||
                c == '`' ||
                c == '\'');
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
    void report(char* format, ...) {
      va_list arg_list;
      va_start(arg_list, format);
      vdprintf(STDERR_FILENO, format, arg_list);
      va_end(arg_list);
    }
      bool string_equal(char* s1, char* s2) {
        if (strcmp(s1, s2) == 0) {
          return true;
        }
        else {
          return false;
        }
      }
      bool nat_string_p(char* str) {
        cell i = 0;
        while (str[i] != 0) {
          if (!isdigit(str[i])) {
            return false;
            }
          i++;
        }
        return true;
      }
      bool int_string_p(char* str) {
        if (str[0] == '-' ||
            str[0] == '+') {
          return nat_string_p(str + 1);
        }
        else {
          return nat_string_p(str);
        }
      }
      cell string_to_based_nat(char* str, cell base) {
        cell result = 0;
        cell len = strlen(str);
        cell i = 0;
        while (i < len) {
          result = result + (char_to_nat(str[i]) * power(base, (len - i - 1)));
          i++;
        }
        return result;
      }

      cell string_to_based_int(char* str, cell base) {
        if (str[0] == '-') {
          return - string_to_based_nat(str, base);
        }
        else {
          return string_to_based_nat(str, base);
        }
      }

      cell string_to_int(char* str) { return string_to_based_int(str, 10); }
      cell string_count_member(char* s, char b) {
        cell sum = 0;
        cell i = 0;
        while (s[i] != '\0') {
          if (s[i] == b) {
            sum++;
          }
          i++;
        }
        return sum;
      }
      bool string_member_p(char* s, char b) {
        cell i = 0;
        while (s[i] != '\0') {
          if (s[i] == b) {
            return true;
          }
          i++;
        }
        return false;
      }
      char string_last_byte(char* s) {
        cell i = 0;
        while (s[i+1] != 0) {
          i++;
        }
        return s[i];
      }
      // caller free
      char* substring(char* str, cell begin, cell end) {
        cell len = strlen(str);
        char* buf = strdup(str);
        buf[end] = '\0';
        if (begin == 0) {
          return buf;
        }
        else {
          char* s = strdup(buf+begin);
          free(buf);
          return s;
        }
      }
      // caller free
      cell* array_len_dup(cell* src, cell len) {
        cell* p = malloc(len * sizeof(cell));
        memcpy(p, src, len * sizeof(cell));
        return p;
      }
      cell array_len(cell* src) {
        cell i = 0;
        while (src[i] != 0) {
          i++;
        }
        return i;
      }
      // caller free
      cell* array_dup(cell* src) {
        return array_len_dup(src, array_len(src) + 1);
      }
      bool array_equal_p(cell* a1, cell* a2) {
        cell i = 0;
        while (true) {
          if (a1[i] == a2[i]) {
            if (a1[i] == 0) {
              return true;
            }
            else {
              // loop
            }
          }
          else {
            return false;
          }
          i++;
        }
      }
  void p_debug();
    struct jotable_entry {
      char *key;
      struct jotable_entry *tag;
      cell data;
    };

    typedef struct jotable_entry* jo_t;

    // prime table size
    //   1000003   about 976 k
    //   1000033
    //   1000333
    //   100003    about 97 k
    //   100333
    //   997
    #define JOTABLE_SIZE 100003
    struct jotable_entry jotable[JOTABLE_SIZE];

    // thus (jotable + index) is jo
    bool used_jo_p(jo_t jo) {
      return jo->tag != 0;
    }
    cell string_to_sum(char* str) {
      cell sum = 0;
      cell max_step = 10;
      cell i = 0;
      while (i < strlen(str)) {
        sum = sum + ((char) str[i]) * (2 << min(i, max_step));
        i++;
      }
      return sum;
    }
    // a hash an index into jotable
    cell jotable_hash(cell sum, cell counter) {
      return (counter + sum) % JOTABLE_SIZE;
    }
    void p_debug();

    jo_t jotable_insert(char* key) {
      // in C : [string] -> [jo]
      cell sum = string_to_sum(key);
      cell counter = 0;
      while (true) {
        cell index = jotable_hash(sum, counter);
        jo_t jo = (jotable + index);
        if (jo->key == 0) {
          key = strdup(key);
          jo->key = key;
          return jo;
        }
        else if (string_equal(key, jo->key)) {
          return jo;
        }
        else if (counter == JOTABLE_SIZE) {
          report("- jotable_insert fail\n");
          report("  the jotable is filled\n");
          p_debug();
        }
        else {
          counter++;
        }
      }
    }
    jo_t str2jo(char* str) {
      return jotable_insert(str);
    }
    char* jo2str(jo_t jo) {
      return jo->key;
    }
    jo_t EMPTY_JO;

    jo_t TAG_PRIM;
    jo_t TAG_JOJO;
    jo_t TAG_CLOSURE;
    jo_t TAG_ADDRESS;
    jo_t TAG_CLASS;

    jo_t TAG_BOOL;
    jo_t TAG_INT;
    jo_t TAG_BYTE;
    jo_t TAG_STRING;
    jo_t TAG_JO;

    jo_t TAG_UNINITIALISED_FIELD_PLACE_HOLDER;

    jo_t ROUND_BAR;
    jo_t ROUND_KET;
    jo_t SQUARE_BAR;
    jo_t SQUARE_KET;
    jo_t FLOWER_BAR;
    jo_t FLOWER_KET;
    jo_t DOUBLEQUOTE;
    jo_t SINGLEQUOTE;
    jo_t QUASIQUOTE;
    jo_t COMMA;

    jo_t JO_INS_LIT;
    jo_t JO_INS_GET_LOCAL;
    jo_t JO_INS_SET_LOCAL;
    jo_t JO_INS_GET_FIELD;
    jo_t JO_INS_SET_FIELD;

    jo_t JO_INS_JMP;
    jo_t JO_INS_JZ;

    jo_t JO_INS_TAIL_CALL;
    jo_t JO_INS_LOOP;
    jo_t JO_INS_RECUR;

    jo_t JO_NULL;
    jo_t JO_THEN;
    jo_t JO_ELSE;

    jo_t JO_APPLY;
    jo_t JO_EXE;
    jo_t JO_END;
    jo_t JO_RECUR;
    jo_t JO_CLOSURE;
    jo_t JO_CURRENT_LOCAL_ENV;

    jo_t JO_LOCAL_DATA_IN;
    jo_t JO_LOCAL_DATA_OUT;

    jo_t JO_LOCAL_TAG_IN;
    jo_t JO_LOCAL_TAG_OUT;

    jo_t JO_LOCAL_IN;
    jo_t JO_LOCAL_OUT;
    jo_t name_record[16 * 1024];
    cell name_record_counter = 0;
    void report_name_record() {
      report("- name_record :\n");
      cell i = 0;
      while (i < name_record_counter) {
        report("  %s\n", jo2str(name_record[i]));
        i++;
      }
    }
    bool core_flag = false;
    void bind_name(jo_t name,
                   jo_t tag,
                   cell data) {
      if (used_jo_p(name) && !core_flag) {
        report("- bind_name can not rebind\n");
        report("  name : %s\n", jo2str(name));
        report("  tag : %s\n", jo2str(tag));
        report("  data : %ld\n", data);
        report("  it has been bound as a %s\n", jo2str(name->tag));
        return;
      }

      name_record[name_record_counter] = name;
      name_record_counter++;
      name_record[name_record_counter] = 0;

      name->tag = tag;
      name->data = data;
    }
    void rebind_name(name, tag, data)
      jo_t name;
      jo_t tag;
      cell data;
    {
      name->tag = tag;
      name->data = data;
    }
      // caller free
      jo_t* generate_jo_array(char*ss[]) {
        cell len = 0;
        while (ss[len] != 0) {
          len++;
        }
        jo_t* js = (jo_t*)malloc(len * sizeof(jo_t) + 1);
        cell i = 0;
        while (i < len) {
          js[i] = str2jo(ss[i]);
          i++;
        }
        js[i] = 0;
        return js;
      }
      #define J0 (char*[]){0}
      #define J(...) generate_jo_array((char*[]){__VA_ARGS__, 0})
    bool jo_bar_ket_p(jo_t jo) {
      char* str = jo2str(jo);
      if (strlen(str) != 1) {
        return false;
      }
      else {
        return char_bar_ket_p(str[0]);
      }
    }
    bool jo_delimiter_p(jo_t jo) {
      char* str = jo2str(jo);
      if (strlen(str) != 1) {
        return false;
      }
      else {
        return char_delimiter_p(str[0]);
      }
    }
    // void report_jotable() {
    //   report("- report_jotable\n");
    //   while () {

    //   }
    // }
    struct stack_link {
      cell* stack;
      struct stack_link* link;
    };
    struct stack {
      char* name;
      cell pointer;
      cell* stack;
      struct stack_link* link;
    };

    #define STACK_BLOCK_SIZE 1024
    // #define STACK_BLOCK_SIZE 1 // for testing
    struct stack* new_stack(char* name) {
      struct stack* stack = (struct stack*)
        malloc(sizeof(struct stack));
      stack->name = name;
      stack->pointer = 0;
      stack->stack = (cell*)malloc(sizeof(cell) * STACK_BLOCK_SIZE);
      stack->link = 0;
      return stack;
    }
    void stack_free_link(struct stack_link* link) {
      if (link == 0) {
        return;
      }
      else {
        stack_free_link(link->link);
        free(link->stack);
        free(link);
      }
    }

    // ><><><
    // stack->name is not freed
    void stack_free(struct stack* stack) {
      stack_free_link(stack->link);
      free(stack->stack);
      free(stack);
    }
    // can not pop
    // for stack->pointer can not decrease under 0
    void stack_block_underflow_check(struct stack* stack) {
      if (stack->pointer > 0) {
        return;
      }
      else if (stack->link != 0) {
        free(stack->stack);
        stack->stack = stack->link->stack;
        struct stack_link* old_link = stack->link;
        stack->link = stack->link->link;
        free(old_link);
        stack->pointer = STACK_BLOCK_SIZE;
        return;
      }
      else {
        report("- stack_block_underflow_check fail\n");
        report("  %s underflow\n", stack->name);
        p_debug();
      }
    }
    // can not push
    // for stack->pointer can not increase over STACK_BLOCK_SIZE
    void stack_block_overflow_check(struct stack* stack) {
      if (stack->pointer < STACK_BLOCK_SIZE) {
        return;
      }
      else {
        struct stack_link* new_link = (struct stack_link*)
          malloc(sizeof(struct stack_link));
        new_link->stack = stack->stack;
        new_link->link = stack->link;
        stack->link = new_link;
        stack->stack = (cell*)malloc(sizeof(cell) * STACK_BLOCK_SIZE);
        stack->pointer = 0;
      }
    }
    bool stack_empty_p(struct stack* stack) {
      return
        stack->pointer == 0 &&
        stack->link == 0;
    }
    cell stack_length_link(cell sum, struct stack_link* link) {
      if (link == 0) {
        return sum;
      }
      else {
        return stack_length_link(sum + STACK_BLOCK_SIZE, link->link);
      }
    }

    cell stack_length(struct stack* stack) {
      return stack_length_link(stack->pointer, stack->link);
    }
    cell pop(struct stack* stack) {
      stack_block_underflow_check(stack);
      stack->pointer--;
      return stack->stack[stack->pointer];
    }
    cell tos(struct stack* stack) {
      stack_block_underflow_check(stack);
      return stack->stack[stack->pointer - 1];
    }
    void drop(struct stack* stack) {
      stack_block_underflow_check(stack);
      stack->pointer--;
    }
    void push(struct stack* stack, cell data) {
      stack_block_overflow_check(stack);
      stack->stack[stack->pointer] = data;
      stack->pointer++;
    }
    cell stack_peek_link(struct stack_link* link, cell index) {
      if (index < STACK_BLOCK_SIZE) {
        return link->stack[STACK_BLOCK_SIZE - index];
      }
      else {
        return stack_peek_link(link->link, index - STACK_BLOCK_SIZE);
      }
    }

    cell stack_peek(struct stack* stack, cell index) {
      if (index <= stack->pointer) {
        return stack->stack[stack->pointer - index];
      }
      else {
        return stack_peek_link(stack->link, index - stack->pointer);
      }
    }
    cell stack_ref(struct stack* stack, cell index) {
      return stack_peek(stack, stack_length(stack) - index);
    }
    typedef enum {
      INPUT_STACK_REGULAR_FILE,
      INPUT_STACK_STRING,
      INPUT_STACK_TERMINAL,
    } input_stack_type;
    struct input_stack_link {
      char* stack;
      cell end_pointer;
      struct input_stack_link* link;
    };
    struct input_stack {
      cell pointer;
      cell end_pointer;
      char* stack;
      struct input_stack_link* link;
      input_stack_type type;
      union {
        int   file;
        char* string;
        // int   terminal;
      };
      cell string_pointer;
    };

    #define INPUT_STACK_BLOCK_SIZE (4 * 1024)
    // #define INPUT_STACK_BLOCK_SIZE 1 // for testing
    struct input_stack* new_input_stack(input_stack_type input_stack_type) {
      struct input_stack* input_stack = (struct input_stack*)
        malloc(sizeof(struct input_stack));
      input_stack->pointer = INPUT_STACK_BLOCK_SIZE;
      input_stack->end_pointer = INPUT_STACK_BLOCK_SIZE;
      input_stack->stack = (char*)malloc(INPUT_STACK_BLOCK_SIZE);
      input_stack->link = 0;
      input_stack->type = input_stack_type;
      return input_stack;
    }
    struct input_stack* file_input_stack(int file) {
      int file_flag = fcntl(file, F_GETFL);
      int access_mode = file_flag & O_ACCMODE;
      if (file_flag == -1) {
        report("- file_input_stack fail\n");
        perror("  fcntl error ");
        p_debug();
      }
      struct input_stack* input_stack = new_input_stack(INPUT_STACK_REGULAR_FILE);
      input_stack->file = file;
      return input_stack;
    }
    struct input_stack* string_input_stack(char* string) {
      struct input_stack* input_stack = new_input_stack(INPUT_STACK_STRING);
      input_stack->string = string;
      input_stack->string_pointer = 0;
      return input_stack;
    }
    struct input_stack* terminal_input_stack() {
      struct input_stack* input_stack = new_input_stack(INPUT_STACK_TERMINAL);
      return input_stack;
    }
    void input_stack_free_link(struct input_stack_link* link) {
      if (link == 0) {
        return;
      }
      else {
        input_stack_free_link(link->link);
        free(link->stack);
        free(link);
      }
    }

    void input_stack_free(struct input_stack* input_stack) {
      input_stack_free_link(input_stack->link);
      free(input_stack->stack);
      free(input_stack);
    }
    // can not pop
    // for input_stack->pointer can not increase over input_stack->end_pointer
    void input_stack_block_underflow_check(struct input_stack* input_stack) {
      if (input_stack->pointer < input_stack->end_pointer) {
        return;
      }

      else if (input_stack->link != 0) {
        free(input_stack->stack);
        input_stack->stack = input_stack->link->stack;
        input_stack->end_pointer = input_stack->link->end_pointer;
        struct input_stack_link* old_link = input_stack->link;
        input_stack->link = input_stack->link->link;
        free(old_link);
        input_stack->pointer = 0;
        return;
      }

      else if (input_stack->type == INPUT_STACK_REGULAR_FILE) {
        ssize_t real_bytes = read(input_stack->file,
                                  input_stack->stack,
                                  INPUT_STACK_BLOCK_SIZE);
        if (real_bytes == 0) {
          report("- input_stack_block_underflow_check fail\n");
          report("  input_stack underflow\n");
          report("  meet end-of-file when reading a regular_file\n");
          report("  file descriptor : %ld\n", input_stack->file);
          p_debug();
        }
        else {
          input_stack->pointer = 0;
          input_stack->end_pointer = real_bytes;
          return;
        }
      }

      else if (input_stack->type == INPUT_STACK_STRING) {
        char byte = input_stack->string[input_stack->string_pointer];
        if (byte == '\0') {
          report("- input_stack_block_underflow_check fail\n");
          report("  input_stack underflow\n");
          report("  meet end-of-string when reading a string\n");
          p_debug();
        }
        input_stack->string_pointer++;
        input_stack->end_pointer = INPUT_STACK_BLOCK_SIZE;
        input_stack->pointer = INPUT_STACK_BLOCK_SIZE - 1;
        input_stack->stack[input_stack->pointer] = byte;
        return;
      }

      else if (input_stack->type == INPUT_STACK_TERMINAL) {
        ssize_t real_bytes = read(STDIN_FILENO,
                                  input_stack->stack,
                                  INPUT_STACK_BLOCK_SIZE);
        if (real_bytes == 0) {
          report("- input_stack_block_underflow_check fail\n");
          report("  input_stack underflow\n");
          report("  meet end-of-file when reading from terminal\n");
          p_debug();
        }
        else {
          input_stack->pointer = 0;
          input_stack->end_pointer = real_bytes;
          return;
        }
      }

      else {
        report("- input_stack_block_underflow_check fail\n");
        report("  meet unknow stack type\n");
        report("  stack type number : %ld\n", input_stack->type);
        p_debug();
      }
    }
    // can not push
    // for input_stack->pointer can not decrease under 0
    void input_stack_block_overflow_check(struct input_stack* input_stack) {
      if (input_stack->pointer > 0) {
        return;
      }
      else {
        struct input_stack_link* new_link = (struct input_stack_link*)
          malloc(sizeof(struct input_stack_link));
        new_link->stack = input_stack->stack;
        new_link->link = input_stack->link;
        new_link->end_pointer = input_stack->end_pointer;
        input_stack->link = new_link;
        input_stack->stack = (char*)malloc(INPUT_STACK_BLOCK_SIZE);
        input_stack->pointer = INPUT_STACK_BLOCK_SIZE;
        input_stack->end_pointer = INPUT_STACK_BLOCK_SIZE;
      }
    }
    bool input_stack_empty_p(struct input_stack* input_stack) {
      if (input_stack->pointer != input_stack->end_pointer ||
          input_stack->link != 0) {
        return false;
      }
      if (input_stack->type == INPUT_STACK_REGULAR_FILE) {
        ssize_t real_bytes = read(input_stack->file,
                                  input_stack->stack,
                                  INPUT_STACK_BLOCK_SIZE);
        if (real_bytes == 0) {
          return true;
        }
        else {
          input_stack->pointer = 0;
          input_stack->end_pointer = real_bytes;
          return false;
        }
      }
      else if (input_stack->type == INPUT_STACK_STRING) {
        return input_stack->string[input_stack->string_pointer] == '\0';
      }
      // else if (input_stack->type == INPUT_STACK_TERMINAL)
      else {
        ssize_t real_bytes = read(STDIN_FILENO,
                                  input_stack->stack,
                                  INPUT_STACK_BLOCK_SIZE);
        if (real_bytes == 0) {
          return true;
        }
        else {
          input_stack->pointer = 0;
          input_stack->end_pointer = real_bytes;
          return false;
        }
      }
    }
    char input_stack_pop(struct input_stack* input_stack) {
      input_stack_block_underflow_check(input_stack);
      char byte = input_stack->stack[input_stack->pointer];
      input_stack->pointer++;
      return byte;
    }
    char input_stack_tos(struct input_stack* input_stack) {
      input_stack_block_underflow_check(input_stack);
      char byte = input_stack->stack[input_stack->pointer];
      return byte;
    }
    void input_stack_drop(struct input_stack* input_stack) {
      input_stack_block_underflow_check(input_stack);
      input_stack->pointer++;
    }
    void input_stack_push(struct input_stack* input_stack, char byte) {
      input_stack_block_overflow_check(input_stack);
      input_stack->pointer--;
      input_stack->stack[input_stack->pointer] = byte;
    }
    typedef enum {
      OUTPUT_STACK_REGULAR_FILE,
      OUTPUT_STACK_STRING,
      OUTPUT_STACK_TERMINAL,
    } output_stack_type;
    struct output_stack_link {
      char* stack;
      struct output_stack_link* link;
    };
    struct output_stack {
      cell pointer;
      char* stack;
      struct output_stack_link* link;
      output_stack_type type;
      union {
        int   file; // with cache
        // char* string;
        //   generate string
        //   instead of output to pre-allocated buffer
        // int   terminal; // no cache
      };
      cell string_pointer;
    };

    #define OUTPUT_STACK_BLOCK_SIZE (4 * 1024)
    // #define OUTPUT_STACK_BLOCK_SIZE 1 // for testing
    struct output_stack* new_output_stack(output_stack_type output_stack_type) {
      struct output_stack* output_stack = (struct output_stack*)
        malloc(sizeof(struct output_stack));
      output_stack->pointer = 0;
      output_stack->stack = (char*)malloc(OUTPUT_STACK_BLOCK_SIZE);
      output_stack->link = 0;
      output_stack->type = output_stack_type;
      return output_stack;
    }
    struct output_stack* file_output_stack(int file) {
      int file_flag = fcntl(file, F_GETFL);
      int access_mode = file_flag & O_ACCMODE;
      if (file_flag == -1) {
        report("- file_output_stack fail\n");
        perror("  fcntl error ");
        p_debug();
      }
      else if (access_mode == O_WRONLY || access_mode == O_RDWR) {
        struct output_stack* output_stack = new_output_stack(OUTPUT_STACK_REGULAR_FILE);
        output_stack->file = file;
        return output_stack;
      }
      else {
        report("- file_output_stack fail\n");
        report("  file_output_stack fail\n");
        p_debug();
      }
    }
    struct output_stack* string_output_stack() {
      struct output_stack* output_stack = new_output_stack(OUTPUT_STACK_STRING);
      return output_stack;
    }
    struct output_stack* terminal_output_stack() {
      struct output_stack* output_stack = new_output_stack(OUTPUT_STACK_TERMINAL);
      return output_stack;
    }
    void output_stack_free_link(struct output_stack_link* link) {
      if (link == 0) {
        return;
      }
      else {
        output_stack_free_link(link->link);
        free(link->stack);
        free(link);
      }
    }

    void output_stack_free(struct output_stack* output_stack) {
      output_stack_free_link(output_stack->link);
      free(output_stack->stack);
      free(output_stack);
    }
    void file_output_stack_flush_link(int file, struct output_stack_link* link) {
      if (link == 0) {
        return;
      }
      else {
        file_output_stack_flush_link(file, link->link);
        ssize_t real_bytes = write(file,
                                   link->stack,
                                   OUTPUT_STACK_BLOCK_SIZE);
        if (real_bytes != OUTPUT_STACK_BLOCK_SIZE) {
          report("- file_output_stack_flush_link fail\n");
          report("  file descriptor : %ld\n", file);
          perror("  write error : ");
          p_debug();
        }
      }
    }

    void file_output_stack_flush(struct output_stack* output_stack) {
      file_output_stack_flush_link(output_stack->file,
                                   output_stack->link);
      ssize_t real_bytes = write(output_stack->file,
                                 output_stack->stack,
                                 output_stack->pointer);
      if (real_bytes != output_stack->pointer) {
        report("- file_output_stack_flush fail\n");
        report("  file descriptor : %ld\n", output_stack->file);
        perror("  write error : ");
        p_debug();
      }
      else {
        output_stack_free_link(output_stack->link);
        output_stack->link = 0;
        output_stack->pointer = 0;
      }
    }
    cell string_output_stack_length_link(cell sum, struct output_stack_link* link) {
      if (link == 0) {
        return sum;
      }
      else {
        return
          OUTPUT_STACK_BLOCK_SIZE +
          string_output_stack_length_link(sum, link->link);
      }
    }

    cell string_output_stack_length(struct output_stack* output_stack) {
      cell sum = strlen(output_stack->stack);
      return string_output_stack_length_link(sum, output_stack->link);
    }


    char* string_output_stack_collect_link(char* buffer, struct output_stack_link* link) {
      if (link == 0) {
        return buffer;
      }
      else {
        buffer = string_output_stack_collect_link(buffer, link->link);
        memcpy(buffer, link->stack, OUTPUT_STACK_BLOCK_SIZE);
        return buffer + OUTPUT_STACK_BLOCK_SIZE;
      }
    }

    char* string_output_stack_collect(struct output_stack* output_stack) {
      char* string = (char*)malloc(1 + string_output_stack_length(output_stack));
      char* buffer = string;
      buffer = string_output_stack_collect_link(buffer, output_stack->link);
      memcpy(buffer, output_stack->stack, output_stack->pointer);
      buffer[output_stack->pointer] = '\0';
      return string;
    }
    // can not pop
    // for output_stack->pointer can not decrease under 0
    void output_stack_block_underflow_check(struct output_stack* output_stack) {
      if (output_stack->pointer > 0) {
        return;
      }

      else if (output_stack->link != 0) {
        free(output_stack->stack);
        output_stack->stack = output_stack->link->stack;
        struct output_stack_link* old_link = output_stack->link;
        output_stack->link = output_stack->link->link;
        free(old_link);
        output_stack->pointer = OUTPUT_STACK_BLOCK_SIZE;
        return;
      }

      else if (output_stack->type == OUTPUT_STACK_REGULAR_FILE) {
        report("- output_stack_block_underflow_check fail\n");
        report("  output_stack underflow\n");
        report("  when writing a regular_file\n");
        report("  file descriptor : %ld\n", output_stack->file);
        p_debug();
      }

      else if (output_stack->type == OUTPUT_STACK_STRING) {
        report("- output_stack_block_underflow_check fail\n");
        report("  output_stack underflow\n");
        report("  when writing a string\n");
        p_debug();
      }

      else if (output_stack->type == OUTPUT_STACK_TERMINAL) {
        report("- output_stack_block_underflow_check fail\n");
        report("  output_stack underflow\n");
        report("  when writing to terminal\n");
        p_debug();
      }

      else {
        report("- output_stack_block_underflow_check fail\n");
        report("  meet unknow stack type\n");
        report("  stack type number : %ld\n", output_stack->type);
        p_debug();
      }
    }
    // can not push
    // for output_stack->pointer can not increase over OUTPUT_STACK_BLOCK_SIZE
    void output_stack_block_overflow_check(struct output_stack* output_stack) {
      if (output_stack->pointer < OUTPUT_STACK_BLOCK_SIZE) {
        return;
      }
      else {
        struct output_stack_link* new_link = (struct output_stack_link*)
          malloc(sizeof(struct output_stack_link));
        new_link->stack = output_stack->stack;
        new_link->link = output_stack->link;
        output_stack->link = new_link;
        output_stack->stack = (char*)malloc(OUTPUT_STACK_BLOCK_SIZE);
        output_stack->pointer = 0;
      }
    }
    bool output_stack_empty_p(struct output_stack* output_stack) {
      if (output_stack->pointer != 0 ||
          output_stack->link != 0) {
        return false;
      }
      if (output_stack->type == OUTPUT_STACK_REGULAR_FILE) {
        return true;
      }
      else if (output_stack->type == OUTPUT_STACK_STRING) {
        return true;
      }
      // else if (output_stack->type == OUTPUT_STACK_TERMINAL)
      else {
        return true;
      }
    }
    char output_stack_pop(struct output_stack* output_stack) {
      output_stack_block_underflow_check(output_stack);
      output_stack->pointer--;
      return output_stack->stack[output_stack->pointer];
    }
    char output_stack_tos(struct output_stack* output_stack) {
      output_stack_block_underflow_check(output_stack);
      return output_stack->stack[output_stack->pointer - 1];
    }
    void output_stack_drop(struct output_stack* output_stack) {
      output_stack_block_underflow_check(output_stack);
      output_stack->pointer--;
    }
    void output_stack_push(struct output_stack* output_stack, char b) {
      if (output_stack->type == OUTPUT_STACK_TERMINAL) {
        char buffer[1];
        buffer[0] = b;
        ssize_t real_bytes = write(STDOUT_FILENO, buffer, 1);
        if (real_bytes != 1) {
          report("- output_stack_push fail\n");
          perror("  write error : ");
          p_debug();
        }
      }
      else {
        output_stack_block_overflow_check(output_stack);
        output_stack->stack[output_stack->pointer] = b;
        output_stack->pointer++;
      }
    }
    struct dp {
      jo_t t;
      cell d;
    };

    struct stack* ds;

    struct dp ds_pop() {
      struct dp p;
      p.t = pop(ds);
      p.d = pop(ds);
      return p;
    }

    struct dp ds_tos() {
      struct dp p;
      p.t = pop(ds);
      p.d = pop(ds);
      push(ds, p.d);
      push(ds, p.t);
      return p;
    }

    bool ds_empty_p() {
      return stack_empty_p(ds);
    }

    void ds_push(jo_t tag, cell data) {
      push(ds, data);
      push(ds, tag);
    }

    jo_t ds_peek_tag(cell index) {
      return stack_peek(ds, (index*2) - 1);
    }

    cell ds_peek_data(cell index) {
      return stack_peek(ds, (index*2));
    }
    struct local {
      jo_t name;
      cell local_tag;
      cell local_data;
    };

    #define LOCAL_RECORD_SIZE (32 * 1024)
    struct local local_record[LOCAL_RECORD_SIZE];
    cell current_local_counter = 0;
    struct rp {
      jo_t* j;
      jo_t  t;
      cell  d;
      cell  l;
    };

    struct stack* rs;

    struct rp rs_pop() {
      struct rp p;
      p.j = pop(rs);
      p.t = pop(rs);
      p.d = pop(rs);
      p.l = pop(rs);
      return p;
    }

    void rs_drop() {
      drop(rs);
      drop(rs);
      drop(rs);
      drop(rs);
    }

    struct rp rs_tos() {
      struct rp p;
      p.j = stack_peek(rs, 1);
      p.t = stack_peek(rs, 2);
      p.d = stack_peek(rs, 3);
      p.l = stack_peek(rs, 4);
      return p;
    }

    bool rs_empty_p() {
      return stack_empty_p(rs);
    }

    void rs_push(jo_t* jojo, jo_t tag, cell data, cell local_counter) {
      push(rs, local_counter);
      push(rs, data);
      push(rs, tag);
      push(rs, jojo);
    }

    cell rs_length() {
      return stack_length(rs) / 4;
    }

    struct rp rs_ref(cell index) {
      struct rp p;
      p.j = stack_ref(rs, index*4 + 3);
      p.t = stack_ref(rs, index*4 + 2);
      p.d = stack_ref(rs, index*4 + 1);
      p.l = stack_ref(rs, index*4 + 0);
      return p;
    }

    void rs_inc() {
      jo_t* jojo = pop(rs);
      push(rs, jojo + 1);
    }
    typedef enum {
      GC_STATE_MARKING,
      GC_STATE_SWEEPING,
    } gc_state_t;
    typedef void (* gc_actor_t)(gc_state_t, cell);
    typedef enum {
      GC_MARK_FREE,
      GC_MARK_USING,
    } gc_mark_t;

    struct gp { // gc point
      gc_mark_t mark;
      gc_actor_t gc_actor;
      cell fields_number;
      cell p; // actual data point
    };

    #define GR_SIZE 64 * 1024
    // #define GR_SIZE 3 // for testing

    struct gp gr[GR_SIZE];

    struct gp* gr_pointer = gr;
    bool gr_end_p() {
      return gr_pointer >= (gr + GR_SIZE);
    }
    void init_gr() {
      bzero(gr, GR_SIZE * sizeof(struct gp));
    }
    struct class {
      jo_t class_name;
      gc_actor_t gc_actor;
      cell fields_number;
      jo_t* fields;
    };
      jo_t get_field_tag(cell* fields, cell field_index) {
        return fields[field_index*2+1];
      }

      void set_field_tag(cell* fields, cell field_index, jo_t tag) {
        fields[field_index*2+1] = tag;
      }

      cell get_field_data(cell* fields, cell field_index) {
        return fields[field_index*2];
      }

      void set_field_data(cell* fields, cell field_index, cell data) {
        fields[field_index*2] = data;
      }
      // assume exist
      jo_t class_index_to_field_name(struct class* class, cell index) {
        return class->fields[index];
      }
      // assume exist
      cell class_field_name_to_index(struct class* class, jo_t field_name) {
        cell i = 0;
        while (i < class->fields_number) {
          if (class->fields[i] == field_name) { return i; }
          i++;
        }
        report("- class_field_name_to_index fail\n");
        report("  field_name : %s\n", jo2str(field_name));
        report("  class_name : %s\n", jo2str(class->class_name));
        p_debug();
      }
      jo_t get_gp_field_tag(gp, field_index)
        struct gp* gp;
        cell field_index;
      {
        cell* fields = gp->p;
        return get_field_tag(fields, field_index);
      }

      void set_gp_field_tag(gp, field_index, tag)
        struct gp* gp;
        cell field_index;
        jo_t tag;
      {
        cell* fields = gp->p;
        set_field_tag(fields, field_index, tag);
      }

      cell get_gp_field_data(gp, field_index)
        struct gp* gp;
        cell field_index;
      {
        cell* fields = gp->p;
        return get_field_data(fields, field_index);
      }

      void set_gp_field_data(gp, field_index, data)
        struct gp* gp;
        cell field_index;
        cell data;
      {
        cell* fields = gp->p;
        set_field_data(fields, field_index, data);
      }
    struct dp get_field(jo_t class_tag, struct gp* gp, jo_t name) {
      struct class* class = class_tag->data;
      cell index = class_field_name_to_index(class, name);
      jo_t tag = get_gp_field_tag(gp, index);
      cell data = get_gp_field_data(gp, index);

      if (tag == TAG_UNINITIALISED_FIELD_PLACE_HOLDER) {
        ds_push(class_tag, gp);
        report("- get_field fail\n");
        report("  field is uninitialised\n");
        report("  field_name : %s\n", jo2str(name));
        report("  class_name : %s\n", jo2str(class->class_name));
        report("  see top of ds for the data\n");
        p_debug();
      }
      else {
        struct dp a;
        a.t = tag;
        a.d = data;
        return a;
      }
    }
    void ins_get_field() {
      struct rp p = rs_tos();
      rs_inc();
      jo_t* jojo = p.j;
      jo_t name = jojo[0];

      struct dp a = ds_pop();
      struct class* class = a.t->data;

      cell index = class_field_name_to_index(class, name);

      jo_t tag = get_gp_field_tag(a.d, index);
      cell data = get_gp_field_data(a.d, index);
      if (tag == TAG_UNINITIALISED_FIELD_PLACE_HOLDER) {
        ds_push(a.t, a.d);
        report("- ins_get_field fail\n");
        report("  field is uninitialised\n");
        report("  field_name : %s\n", jo2str(name));
        report("  class_name : %s\n", jo2str(class->class_name));
        report("  see top of ds for the data\n");
        p_debug();
      }
      else {
        ds_push(tag, data);
      }
    }
    void ins_set_field() {
      struct rp p = rs_tos();
      rs_inc();
      jo_t* jojo = p.j;
      jo_t name = jojo[0];

      struct dp a = ds_pop();
      struct class* class = a.t->data;

      cell index = class_field_name_to_index(class, name);

      struct dp b = ds_pop();
      set_gp_field_tag(a.d, index, b.t);
      set_gp_field_data(a.d, index, b.d);
    }
    void mark_one(jo_t tag, cell data) {
      struct class* class = tag->data;
      class->gc_actor(GC_STATE_MARKING, data);
    }
    void mark_gr() {
      // prepare
      cell i = 0;
      while (i < GR_SIZE) {
        gr[i].mark = GC_MARK_FREE;
        i++;
      }
      // name_record as root
      i = 0;
      while (i < name_record_counter) {
        jo_t name = name_record[i];
        mark_one(name->tag, name->data);
        i++;
      }
      // ds as root
      i = 0;
      while (i < stack_length(ds)) {
        mark_one(stack_ref(ds, i+1),
                 stack_ref(ds, i));
        i++;
        i++;
      }
      // local_record as root
      i = 0;
      while (i < current_local_counter) {
        mark_one(local_record[i].local_tag,
                 local_record[i].local_data);
        i++;
      }
    }
    void sweep_one(struct gp* gp) {
      if (gp->mark == GC_MARK_USING) {
        return;
      }
      else {
        gp->gc_actor(GC_STATE_SWEEPING, gp);
      }
    }
    void sweep_gr() {
      cell i = 0;
      while (i < GR_SIZE) {
        sweep_one(gr + i);
        i++;
      }
    }
      void gc_ignore(gc_state_t gc_state, cell data) {
        if (gc_state == GC_STATE_MARKING) {
        }
        else if (gc_state == GC_STATE_SWEEPING) {
        }
      }
      void gc_free(gc_state_t gc_state, struct gp* gp) {
        if (gc_state == GC_STATE_MARKING) {
          gp->mark = GC_MARK_USING;
        }
        else if (gc_state == GC_STATE_SWEEPING) {
          free(gp->p);
        }
      }
      void gc_recur(gc_state_t gc_state, struct gp* gp) {
        if (gc_state == GC_STATE_MARKING) {
          if (gp->mark == GC_MARK_USING) { return; }
          gp->mark = GC_MARK_USING;
          cell fields_number = gp->fields_number;
          cell* fields = gp->p;
          cell i = 0;
          while (i < fields_number) {
            mark_one(get_gp_field_tag(gp, i),
                     get_gp_field_data(gp, i));
            i++;
          }
        }
        else if (gc_state == GC_STATE_SWEEPING) {
          free(gp->p);
        }
      }
    void run_gc() {
      mark_gr();
      sweep_gr();
    }

    // run_gc() {
    //   report("- run_gc()\n");
    //   mark_gr();
    //   report("- after mark_gr()\n");
    //   sweep_gr();
    //   report("- after sweep_gr()\n");
    // }
    void next_free_record_gp() {
      while (!gr_end_p() &&
             gr_pointer->mark != GC_MARK_FREE) {
        gr_pointer++;
      }
    }

    struct gp* new_record_gp() {
      next_free_record_gp();
      if (!gr_end_p()) {
        return gr_pointer++;
      }
      else {
        run_gc();
        gr_pointer = gr;
        if (!gr_end_p()) {
          return gr_pointer++;
        }
        else {
          report("- new_record_gp fail\n");
          report("  after gc, the gr is still filled\n");
          report("  GR_SIZE : %ld\n", GR_SIZE);
          return 0;
        }
      }
    }
    struct gp* new_static_gp() {
      struct gp* gp = (struct gp*)
        malloc(sizeof(struct gp));
      return gp;
    }
    struct gp* new(struct class* class) {
      cell* fields = (cell*)malloc(class->fields_number*2*sizeof(cell));

      cell i = 0;
      while (i < class->fields_number) {
        set_field_tag(fields, i, str2jo("<uninitialised-field-place-holder>"));
        i++;
      }

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_recur;
      gp->p = fields;
      gp->fields_number = class->fields_number;

      return gp;
    }
    void plus_atom(class_name, gc_actor)
      char* class_name;
      gc_actor_t gc_actor;
    {
      struct class* class = (struct class*)
        malloc(sizeof(struct class));
      class->class_name = str2jo(class_name);
      class->gc_actor = gc_actor;

      jo_t name = str2jo(class_name);
      bind_name(name, TAG_CLASS, class);

      char* tmp = substring(class_name, 1, strlen(class_name) -1);
      jo_t data_constructor_name = str2jo(tmp);
      free(tmp);

      char* tmp2 = malloc(strlen(jo2str(data_constructor_name) + 1 + 1));
      tmp2[0] = '\0';
      strcat(tmp2, jo2str(data_constructor_name));
      strcat(tmp2, "?");
      jo_t data_predicate_name = str2jo(tmp2);
      free(tmp2);

      bind_name(data_predicate_name, str2jo("<data-predicate>"), class);
    }
      void plus_data(class_name, fields)
        char* class_name;
        jo_t* fields[];
      {
        struct class* class = (struct class*)
          malloc(sizeof(struct class));
        jo_t name = str2jo(class_name);
        class->class_name = name;
        class->gc_actor = gc_recur;

        cell i = 0;
        while (fields[i] != 0) {
          i++;
        }

        class->fields_number = i;
        class->fields = fields;

        bind_name(name, TAG_CLASS, class);

        char* tmp = substring(class_name, 1, strlen(class_name) -1);
        jo_t data_constructor_name = str2jo(tmp);
        free(tmp);

        bind_name(data_constructor_name, str2jo("<data-constructor>"), class);

        char* tmp2 = malloc(strlen(jo2str(data_constructor_name) + 1 + 1));
        tmp2[0] = '\0';
        strcat(tmp2, jo2str(data_constructor_name));
        strcat(tmp2, "?");
        jo_t data_predicate_name = str2jo(tmp2);
        free(tmp2);

        bind_name(data_predicate_name, str2jo("<data-predicate>"), class);
      }
      void _plus_data(name, fields)
        jo_t name;
        jo_t fields[];
      {
        plus_data(jo2str(name), fields);
      }
    void plus_prim(function_name, fun)
         char* function_name;
         primitive_t fun;
    {
      jo_t name = str2jo(function_name);
      bind_name(name, TAG_PRIM, fun);
    }
    void p_tag() {
      struct dp a = ds_pop();
      ds_push(TAG_JO, a.t);
    }
    void p_eq_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, (b.t == a.t) && (b.d == a.d));
    }
    void expose_gc() {
      init_gr();

      plus_prim("ins/get-field", ins_get_field);
      plus_prim("ins/set-field", ins_set_field);

      plus_atom("<byte>", gc_ignore);
      plus_atom("<int>", gc_ignore);
      plus_atom("<jo>", gc_ignore);
      plus_atom("<string>", gc_free);
      plus_atom("<gene>", gc_ignore);
      plus_atom("<uninitialised-field-place-holder>", gc_ignore);

      plus_atom("<prim>", gc_ignore);
      plus_atom("<jojo>", gc_ignore);
      plus_atom("<address>", gc_ignore);
      plus_atom("<set-global-variable>", gc_ignore);
      plus_atom("<data-constructor>", gc_ignore);
      plus_atom("<data-predicate>", gc_ignore);

      plus_prim("tag", p_tag);
      plus_prim("eq?", p_eq_p);
    }
      struct disp_entry {
        jo_t key;
        jo_t tag;
        cell data;
        struct disp_entry* rest;
      };

      struct disp {
        struct disp_entry* table;
        cell size;
      };
      struct disp* new_disp(cell size) {
        struct disp* disp = (struct disp*)
          malloc(sizeof(struct disp));
        disp->size = size;
        disp->table = (struct disp_entry*)
          malloc(size * sizeof(struct disp_entry));
        bzero(disp->table, size * sizeof(struct disp_entry));
        return disp;
      }
      cell disp_hash(struct disp* disp, jo_t key) {
        // return (((key - jotable) >> 1)
        //         % (disp->size - 1)) + 1;
        return ((key - jotable)
                % (disp->size - 1)) + 1;
      }
      void disp_insert_entry(disp_entry, key, tag, data)
           struct disp_entry* disp_entry;
           jo_t key;
           jo_t tag;
           cell data;
      {
        if (0 == disp_entry->key) {
          disp_entry->key = key;
          disp_entry->tag = tag;
          disp_entry->data = data;
        }
        else if (key == disp_entry->key) {
          disp_entry->tag = tag;
          disp_entry->data = data;
        }
        else if (disp_entry->rest == 0) {
          struct disp_entry* disp_entry_new = (struct disp_entry*)
            malloc(sizeof(struct disp_entry));
          bzero(disp_entry_new, sizeof(struct disp_entry));
          disp_entry->rest = disp_entry_new;
          disp_insert_entry(disp_entry_new, key, tag, data);
        }
        else {
          disp_insert_entry(disp_entry->rest, key, tag, data);
        }
      }
      void disp_insert(struct disp* disp, jo_t key, jo_t tag, cell data) {
        cell index = disp_hash(disp, key);
        struct disp_entry* disp_entry = disp->table + index;
        disp_insert_entry(disp_entry, key, tag, data);
      }
      struct disp_entry*
      disp_find_entry(disp_entry, key)
           struct disp_entry* disp_entry;
           jo_t key;
      {
        if (key == disp_entry->key) {
          return disp_entry;
        }
        else if (disp_entry->rest != 0) {
          return disp_find_entry(disp_entry->rest, key);
        }
        else {
          return 0;
        }
      }
      struct disp_entry*
      disp_find(disp, key)
           struct disp* disp;
           jo_t key;
      {
        cell index = disp_hash(disp, key);
        struct disp_entry* disp_entry = disp->table + index;
        return disp_find_entry(disp_entry, key);
      }
      void disp_print_entry(struct disp_entry* disp_entry) {
        if (disp_entry->key != 0) {
          report("{%s = %s %ld} ",
                 jo2str(disp_entry->key),
                 jo2str(disp_entry->tag),
                 disp_entry->data);
        }
        if (disp_entry->rest != 0) {
          disp_print_entry(disp_entry->rest);
        }
      }
      void disp_print(struct disp* disp) {
        report("- disp_print\n");
        cell i = 0;
        while (i < disp->size) {
          struct disp_entry* disp_entry = disp->table + i;
          if (disp_entry->key != 0) {
            report("  ");
            disp_print_entry(disp_entry);
            report("\n");
          }
          i++;
        }
      }
      struct multi_disp_entry {
        jo_t* key;
        jo_t tag;
        cell data;
        struct multi_disp_entry* rest;
      };

      struct multi_disp {
        struct multi_disp_entry* table;
        cell size;
      };
      struct multi_disp* new_multi_disp(cell size) {
        struct multi_disp* multi_disp = (struct multi_disp*)
          malloc(sizeof(struct multi_disp));
        multi_disp->size = size;
        multi_disp->table = (struct multi_disp_entry*)
          malloc(size * sizeof(struct multi_disp_entry));
        bzero(multi_disp->table, size * sizeof(struct multi_disp_entry));
        return multi_disp;
      }
      cell multi_disp_hash(struct multi_disp* multi_disp, jo_t* key) {
        cell sum = 0;
        cell i = 0;
        while (key[i] != 0) {
          sum = sum + (key[i] - jotable);
          i++;
        }
        return (sum
                % (multi_disp->size - 1)) + 1;
      }
      void multi_disp_insert_entry(multi_disp_entry, key, tag, data)
           struct multi_disp_entry* multi_disp_entry;
           jo_t* key;
           jo_t tag;
           cell data;
      {
        if (0 == multi_disp_entry->key) {
          multi_disp_entry->key = array_dup(key);
          multi_disp_entry->tag = tag;
          multi_disp_entry->data = data;
        }
        else if (array_equal_p(key, multi_disp_entry->key)) {
          multi_disp_entry->tag = tag;
          multi_disp_entry->data = data;
        }
        else if (multi_disp_entry->rest == 0) {
          struct multi_disp_entry* multi_disp_entry_new = (struct multi_disp_entry*)
            malloc(sizeof(struct multi_disp_entry));
          bzero(multi_disp_entry_new, sizeof(struct multi_disp_entry));
          multi_disp_entry->rest = multi_disp_entry_new;
          multi_disp_insert_entry(multi_disp_entry_new, key, tag, data);
        }
        else {
          multi_disp_insert_entry(multi_disp_entry->rest, key, tag, data);
        }
      }
      void multi_disp_insert(multi_disp, key, tag, data)
           struct multi_disp* multi_disp;
           jo_t* key;
           jo_t tag;
           cell data;
      {
        cell index = multi_disp_hash(multi_disp, key);
        struct multi_disp_entry* multi_disp_entry = multi_disp->table + index;
        multi_disp_insert_entry(multi_disp_entry, key, tag, data);
      }
      struct multi_disp_entry*
      multi_disp_find_entry(multi_disp_entry, key)
           struct multi_disp_entry* multi_disp_entry;
           jo_t* key;
      {
        if (multi_disp_entry->key == 0) {
          return 0;
        }
        else if (array_equal_p(key, multi_disp_entry->key)) {
          return multi_disp_entry;
        }
        else if (multi_disp_entry->rest != 0) {
          return multi_disp_find_entry(multi_disp_entry->rest, key);
        }
        else {
          return 0;
        }
      }
      struct multi_disp_entry*
      multi_disp_find(multi_disp, key)
           struct multi_disp* multi_disp;
           jo_t* key;
      {
        cell i = 0;
        // {
        //   report("- multi_disp_find\n");
        //   while (key[i] != 0) {
        //     report("  \"%s\"\n", jo2str(key[i]));
        //     i++;
        //   }
        // }
        cell index = multi_disp_hash(multi_disp, key);
        struct multi_disp_entry* multi_disp_entry = multi_disp->table + index;
        return multi_disp_find_entry(multi_disp_entry, key);
      }
      void multi_disp_print_entry(struct multi_disp_entry* multi_disp_entry) {
        if (multi_disp_entry->key != 0) {
          report("{");
          cell i = 0;
          while (multi_disp_entry->key[i] != 0) {
            report("%s ", jo2str(multi_disp_entry->key[i]));
            i++;
          }
          report("= %s %ld} ",
                 jo2str(multi_disp_entry->tag),
                 multi_disp_entry->data);
        }
        if (multi_disp_entry->rest != 0) {
          multi_disp_print_entry(multi_disp_entry->rest);
        }
      }
      void multi_disp_print(struct multi_disp* multi_disp) {
        report("- multi_disp_print\n");
        cell i = 0;
        while (i < multi_disp->size) {
          struct multi_disp_entry* multi_disp_entry = multi_disp->table + i;
          if (multi_disp_entry->key != 0) {
            report("  ");
            multi_disp_print_entry(multi_disp_entry);
            report("\n");
          }
          i++;
        }
      }
    struct gene {
      union {
        jo_t disp;
        jo_t* multi_disp;
      };
      cell arity;
    };
    void plus_gene(function_name, arity)
         char* function_name;
         cell arity;
    {
      jo_t name = str2jo(function_name);
      struct gene* gene = (struct gene*)
        malloc(sizeof(struct gene));
      gene->arity = arity;

      if (arity == 1) {
        struct disp* disp = new_disp(128);
        gene->disp = disp;
      }
      else {
        struct multi_disp* multi_disp = new_multi_disp(128);
        gene->multi_disp = multi_disp;
      }

      bind_name(name, str2jo("<gene>"), gene);
    }
    void plus_disp(gene_name, tags, tag_name, data)
      char* gene_name;
      jo_t* tags;
      char* tag_name;
      cell data;
    {
      jo_t name = str2jo(gene_name);
      jo_t tag = str2jo(tag_name);
      struct gene* gene = name->data;
      if (gene->arity == 1) {
        disp_insert(gene->disp, tags[0], tag, data);
      }
      else {
        multi_disp_insert(gene->multi_disp, tags, tag, data);
      }
    }
    void _plus_disp(gene_name, tags, tag_name, data)
      jo_t gene_name;
      jo_t* tags;
      jo_t tag_name;
      cell data;
    {
      plus_disp(jo2str(gene_name), tags, jo2str(tag_name), data);
    }
    void disp_exe(struct gene* gene, jo_t tag) {
      struct disp* disp = gene->disp;
      struct disp_entry* disp_entry =
        disp_find(disp, tag);
      if (disp_entry == 0) {
        report("- disp_exe meet unknow tag\n");
        report("  tag : %s\n", jo2str(tag));
        disp_print(disp);
        p_debug();
        return;
      }
      else {
        if (disp_entry->tag == TAG_PRIM) {
          primitive_t f = (primitive_t)disp_entry->data;
          f();
        }
        else {
          ds_push(disp_entry->tag, disp_entry->data);
          disp_exe(JO_EXE->data, disp_entry->tag);
        }
      }
    }
    void disp_exe_for_jo_apply(struct gene* gene, jo_t tag) {
      struct disp* disp = gene->disp;
      struct disp_entry* disp_entry =
        disp_find(disp, tag);
      if (disp_entry == 0) {
        return;
      }
      else {
        if (disp_entry->tag == TAG_PRIM) {
          primitive_t f = (primitive_t)disp_entry->data;
          f();
        }
        else {
          ds_push(disp_entry->tag, disp_entry->data);
          disp_exe(JO_EXE->data, disp_entry->tag);
        }
      }
    }
    void multi_disp_exe(struct gene* gene, jo_t* tags) {
      struct multi_disp* multi_disp = gene->multi_disp;
      struct multi_disp_entry* multi_disp_entry =
        multi_disp_find(multi_disp, tags);
      if (multi_disp_entry == 0) {
        report("- multi_disp_exe meet unknow tags\n");
        report("  tags : ");
        cell i = 0;
        while (tags[i] != 0) {
          report("%s ", jo2str(tags[i]));
          i++;
        }
        report("  \n");
        multi_disp_print(multi_disp);
        p_debug();
        return;
      }
      else {
        if (multi_disp_entry->tag == TAG_PRIM) {
          primitive_t f = (primitive_t)multi_disp_entry->data;
          f();
        }
        else {
          ds_push(multi_disp_entry->tag, multi_disp_entry->data);
          disp_exe(JO_EXE->data, multi_disp_entry->tag);
        }
      }
    }
    void p_gene_exe() {
      struct dp a = ds_pop();
      struct gene* gene = a.d;
      if (gene->arity == 1) {
        struct dp t = ds_tos();
        disp_exe(gene, t.t);
      }
      else {
        jo_t tags[16];
        cell i = 0;
        while (i < gene->arity) {
          tags[i] = ds_peek_tag(gene->arity - i);
          i++;
        }
        tags[i] = 0;
        multi_disp_exe(gene, tags);
      }
    }
    void p_prim_exe() {
      struct dp a = ds_pop();
      primitive_t f = (primitive_t)a.d;
      f();
    }
    void p_jojo_exe() {
      struct dp a = ds_pop();
      jo_t* jojo = a.d;
      rs_push(jojo, TAG_JOJO, jojo, current_local_counter);
    }
    void p_set_global_variable_exe() {
      struct dp b = ds_pop();
      jo_t name = b.d;
      struct dp a = ds_pop();
      rebind_name(name, a.t, a.d);
    }
    void p_data_constructor_exe() {
      struct dp b = ds_pop();
      struct class* class = b.d;

      cell* fields = (cell*)malloc(class->fields_number*2*sizeof(cell));

      cell i = 0;
      while (i < class->fields_number) {
        struct dp a = ds_pop();
        set_field_tag(fields, (class->fields_number - (i+1)), a.t);
        set_field_data(fields, (class->fields_number - (i+1)), a.d);
        i++;
      }

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_recur;
      gp->p = fields;
      gp->fields_number = class->fields_number;

      ds_push(class->class_name, gp);
    }
    void p_data_predicate_exe() {
      struct dp b = ds_pop();
      struct class* class = b.d;

      struct dp a = ds_pop();
      ds_push(TAG_BOOL, (class->class_name == a.t));
    }
    jo_t* jojo_of(char* function_name) {
      jo_t name = str2jo(function_name);
      return name->data;
    }

    void expose_gene() {
      plus_gene("exe", 1);

      plus_disp("exe", J("<prim>"), "<prim>", p_prim_exe);
      plus_disp("exe", J("<jojo>"), "<prim>", p_jojo_exe);
      plus_disp("exe", J("<gene>"), "<prim>", p_gene_exe);
      plus_disp("exe", J("<set-global-variable>"),
               "<prim>", p_set_global_variable_exe);
      plus_disp("exe", J("<data-constructor>"),
               "<prim>", p_data_constructor_exe);
      plus_disp("exe", J("<data-predicate>"),
               "<prim>", p_data_predicate_exe);
    }
    void p_debug();

    void jo_apply(jo_t jo) {
      if (!used_jo_p(jo)) {
        report("- jo_apply meet undefined jo : %s\n", jo2str(jo));
        p_debug();
        return;
      }
      ds_push(jo->tag, jo->data);
      disp_exe_for_jo_apply(JO_EXE->data, jo->tag);
    }
    void eval() {
      cell base = rs->pointer;
      while (rs->pointer >= base) {
        struct rp p = rs_tos();
        jo_t* jojo = p.j;
        jo_t jo = jojo[0];
        jo_t next_jo = jojo[1];
        if (next_jo == JO_END) {
          // tail call is handled here
          rs_drop();
          current_local_counter = p.l;
          if (jo == JO_RECUR) {
            ds_push(p.t, p.d);
            disp_exe(JO_EXE->data, p.t);
          }
          else {
            jo_apply(jo);
          }
        }
        else {
          rs_inc();
          jo_apply(jo);
        }
      }
    }
    void p_end() {
      // for 'p_step' which do not handle tail call
      struct rp p = rs_pop();
      current_local_counter = p.l;
    }
    void p_bye() {
      report("bye bye ^-^/\n");
      exit(0);
    }
    void expose_ending() {
      plus_prim("end", p_end);
      plus_prim("bye", p_bye);
    }
    void p_drop() {
      ds_pop();
    }
    void p_dup() {
      struct dp a = ds_pop();
      ds_push(a.t, a.d);
      ds_push(a.t, a.d);
    }
    void p_over() {
      // b a -> b a b
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(b.t, b.d);
      ds_push(a.t, a.d);
      ds_push(b.t, b.d);
    }
    void p_tuck() {
      // b a -> a b a
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(a.t, a.d);
      ds_push(b.t, b.d);
      ds_push(a.t, a.d);
    }
    void p_swap() {
      // b a -> a b
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(a.t, a.d);
      ds_push(b.t, b.d);
    }
    void expose_stack() {
      plus_prim("drop", p_drop);
      plus_prim("dup",  p_dup);
      plus_prim("over", p_over);
      plus_prim("tuck", p_tuck);
      plus_prim("swap", p_swap);
    }
    struct stack* reading_stack; // of input_stack
    struct stack* writing_stack; // of output_stack
    bool has_byte_p() {
      return !input_stack_empty_p(tos(reading_stack));
    }
    char read_byte() {
      return input_stack_pop(tos(reading_stack));
    }
    void byte_unread(char b) {
      input_stack_push(tos(reading_stack), b);
    }
    void byte_write(char b) {
      output_stack_push(tos(writing_stack), b);
    }
    bool has_jo_p() {
      char c;
      while (true) {
        if (!has_byte_p()) {
          return false;
        }
        c = read_byte();
        if (char_space_p(c)) {
          // loop
        }
        else {
          byte_unread(c);
          return true;
        }
      }
    }
    void p_has_jo_p() {
      ds_push(TAG_BOOL, has_jo_p());
    }
    jo_t read_jo() {
      char buf[1024];
      cell cur = 0;
      cell collecting = false;
      char c;
      char go = true;

      while (go) {

        if (!has_byte_p()) {
          if (!collecting) {
            report("- p_read_jo fail\n");
            report("  meet end-of-file when still collecting bytes\n");
            p_debug();
          }
          else {
            break;
          }
        }

        c = read_byte(); // report("- read_byte() : %c\n", c);

        if (collecting) {
          if (char_delimiter_p(c) ||
              char_space_p(c)) {
            byte_unread(c);
            go = false;
          }
          else {
            buf[cur] = c;
            cur++;
          }
        }

        else {
          if (char_space_p(c)) {
            // loop
          }
          else {
            collecting = true;
            buf[cur] = c;
            cur++;
            if (char_delimiter_p(c)) {
              go = false;
            }
          }
        }
      }

      buf[cur] = 0;
      return str2jo(buf);
    }
    void p_read_jo() {
      ds_push(TAG_JO, read_jo());
    }
    void string_unread(char* str) {
      if (str[0] == '\0') {
        return;
      }
      else {
        string_unread(str+1);
        byte_unread(str[0]);
      }
    }
    void jo_unread(jo_t jo) {
      char* str = jo2str(jo);
      // byte_unread(' ');
      string_unread(str);
      byte_unread(' ');
    }
    void p_newline() {
      output_stack_push(tos(writing_stack), '\n');
    }
    void p_space() {
      output_stack_push(tos(writing_stack), ' ');
    }
    void expose_rw() {
      plus_prim("has-jo?", p_has_jo_p);
      plus_prim("read-jo", p_read_jo);
      plus_prim("newline", p_newline);
      plus_prim("space", p_space);
    }
    void p_reading_stack_push() {
      struct dp a = ds_pop();
      push(reading_stack, a.d);
    }
    void p_reading_stack_tos() {
      ds_push(str2jo("<input-stack>"), tos(reading_stack));
    }
    void p_reading_stack_pop() {
      ds_push(str2jo("<input-stack>"), pop(reading_stack));
    }
    void p_reading_stack_drop() {
      drop(reading_stack);
    }
    void p_terminal_input_stack() {
      ds_push(str2jo("<input-stack>"), terminal_input_stack());
    }
    void p_input_stack_free() {
      struct dp a = ds_pop();
      input_stack_free(a.d);
    }
    void expose_io() {
      plus_atom("<input-stack>", gc_ignore);


      plus_prim("reading-stack-push", p_reading_stack_push);
      plus_prim("reading-stack-tos", p_reading_stack_tos);
      plus_prim("reading-stack-pop", p_reading_stack_pop);
      plus_prim("reading-stack-drop", p_reading_stack_drop);

      plus_prim("terminal-input-stack", p_terminal_input_stack);

      plus_prim("input-stack-free", p_input_stack_free);
    }
    cell local_find(jo_t name) {
      // return index of local_record
      // -1 -- no found
      struct rp p = rs_tos();
      cell cursor = current_local_counter - 1;
      while (cursor >= p.l) {
        if (local_record[cursor].name == name) {
          return cursor;
        }
        else {
          cursor--;
        }
      }
      return -1;
    }
    void set_local(jo_t name, jo_t tag, cell data) {
      if (current_local_counter < LOCAL_RECORD_SIZE) {
        local_record[current_local_counter].name = name;
        local_record[current_local_counter].local_tag = tag;
        local_record[current_local_counter].local_data = data;
        current_local_counter++;
      }
      else {
        report("- set_local fail\n");
        report("  local_record is filled\n");
        report("  LOCAL_RECORD_SIZE : %ld\n", LOCAL_RECORD_SIZE);
        report("  name : %s\n", jo2str(name));
        report("  tag : %s\n", jo2str(tag));
        report("  data : %ld\n", data);
        p_debug();
      }
    }
    void ins_set_local() {
      struct rp p = rs_tos();
      rs_inc();
      jo_t* jojo = p.j;
      jo_t name = jojo[0];

      struct dp a = ds_pop();
      set_local(name, a.t, a.d);
    }
    void ins_get_local() {
      struct rp p = rs_tos();
      rs_inc();
      jo_t* jojo = p.j;
      jo_t name = jojo[0];

      cell index = local_find(name);

      if (index != -1) {
        struct local lp = local_record[index];
        ds_push(lp.local_tag, lp.local_data);
      }
      else {
        report("- ins_get_local fatal error\n");
        report("  name is not bound\n");
        report("  name : %s\n", jo2str(name));
        p_debug();
      }
    }
    void expose_local() {
      plus_prim("ins/get-local", ins_get_local);
      plus_prim("ins/set-local", ins_set_local);
    }
    struct stack* compiling_stack; // of jojo

    void p_compiling_stack_inc() {
      jo_t* jojo = pop(compiling_stack);
      push(compiling_stack, jojo + 1);
    }
    void emit(cell n) {
      jo_t* jojo = pop(compiling_stack);
      jojo[0] = n;
      push(compiling_stack, jojo + 1);
    }
      // :local
      bool get_local_string_p(char* str) {
        if (str[0] != ':') {
          return false;
        }
        else if (string_last_byte(str) == '!') {
          return false;
        }
        else if (string_member_p(str, '.')) {
          return false;
        }
        else {
          return true;
        }
      }
      // :local!
      bool set_local_string_p(char* str) {
        if (str[0] != ':') {
          return false;
        }
        else if (string_last_byte(str) != '!') {
          return false;
        }
        else if (string_member_p(str, '.')) {
          return false;
        }
        else {
          return true;
        }
      }
      // .field
      bool get_field_string_p(char* str) {
        if (str[0] != '.') {
          return false;
        }
        else if (string_last_byte(str) == '!') {
          return false;
        }
        else if (string_count_member(str, '.') != 1) {
          return false;
        }
        else {
          return true;
        }
      }
      // .field!
      bool set_field_string_p(char* str) {
        if (str[0] != '.') {
          return false;
        }
        else if (string_last_byte(str) != '!') {
          return false;
        }
        else if (string_count_member(str, '.') != 1) {
          return false;
        }
        else {
          return true;
        }
      }
      // <tag>
      bool tag_string_p(char* str) {
        if (str[0] != '<') {
          return false;
        }
        else if (string_last_byte(str) != '>') {
          return false;
        }
        else {
          return true;
        }
      }
    void compile_string() {
      // "..."
      char buffer[1024 * 1024];
      cell cursor = 0;
      while (true) {
        char c = read_byte();
        if (c == '"') {
          buffer[cursor] = '\0';
          cursor++;
          break;
        }
        else {
          buffer[cursor] = c;
          cursor++;
        }
      }
      char* str = strdup(buffer);
      struct gp* gp = new_static_gp();
      gp->gc_actor = gc_ignore;
      gp->p = str;

      emit(JO_INS_LIT);
      emit(TAG_STRING);
      emit(gp);
    }
    void k_closure();

    void compile_jo(jo_t jo) {
      if (jo == ROUND_BAR) {
        jo_apply(read_jo());
        return;
      }

      char* str = jo2str(jo);
      // number
      if (int_string_p(str)) {
        emit(JO_INS_LIT);
        emit(TAG_INT);
        emit(string_to_int(str));
      }
      // "string"
      else if (jo == DOUBLEQUOTE) {
        compile_string();
      }
      // :local
      else if (get_local_string_p(str)) {
        emit(JO_INS_GET_LOCAL);
        emit(jo);
      }
      // :local!
      else if (set_local_string_p(str)) {
        emit(JO_INS_SET_LOCAL);
        char* tmp = substring(str, 0, strlen(str) -1);
        emit(str2jo(tmp));
        free(tmp);
      }
      // .field
      else if (get_field_string_p(str)) {
        emit(JO_INS_GET_FIELD);
        emit(jo);
      }
      // .field!
      else if (set_field_string_p(str)) {
        emit(JO_INS_SET_FIELD);
        char* tmp = substring(str, 0, strlen(str) -1);
        emit(str2jo(tmp));
        free(tmp);
      }
      // ,
      else if (jo == COMMA) {
        // ignore
      }
      // ' jo
      else if (str[0] == '\'') {
        jo_t next_jo = read_jo();
        if (jo_bar_ket_p(next_jo)) {
          report("- compile_jo fail\n");
          report("  can not handle bar-ket after ' in this reader\n");
          report("  can only handle ' jo\n");
          report("  delimiter : %s\n", jo2str(next_jo));
          p_debug();
        }
        else {
          emit(JO_INS_LIT);
          emit(TAG_JO);
          emit(next_jo);
        }
      }
      // {...}
      else if (jo == FLOWER_BAR) {
        k_closure();
      }
      else {
        emit(jo);
      }
    }
    void compile_until_meet_jo(jo_t ending_jo) {
      while (true) {
        jo_t jo = read_jo();
        if (jo == ending_jo) { return; }
        compile_jo(jo);
      }
    }
    jo_t compile_until_meet_jo_or_jo(jo_t ending_jo1, jo_t ending_jo2) {
      while (true) {
        jo_t jo = read_jo();
        if (jo == ending_jo1 || jo == ending_jo2) {
          return jo;
        }
        else {
          compile_jo(jo);
        }
      }
    }
    void p_compile_until_round_ket() {
      compile_until_meet_jo(ROUND_KET);
    }
    void expose_compiler() {

    }
    void k_ignore() {
      while (true) {
        jo_t s = read_jo();
        if (s == ROUND_BAR) {
          k_ignore();
        }
        if (s == ROUND_KET) {
          break;
        }
      }
    }
    void ins_lit() {
      struct rp p = rs_tos();
      rs_inc();
      rs_inc();
      jo_t* jojo = p.j;
      jo_t tag = jojo[0];
      cell data = jojo[1];
      ds_push(tag, data);
    }
    void ins_jmp() {
      struct rp p = rs_pop();
      jo_t* jojo = p.j;
      cell offset = jojo[0];
      rs_push(jojo + offset, p.t, p.d, p.l);
    }
    void ins_jz() {
      struct rp p = rs_tos();
      rs_inc();
      jo_t* jojo = p.j;
      cell offset = jojo[0];
      struct dp a = ds_pop();
      if (a.t == TAG_BOOL && a.d == false) {
        struct rp q = rs_pop();
        rs_push(jojo + offset, p.t, p.d, q.l);
      }
    }
    //// if then
    //   (if a b p? then c d)
    //// ==>
    //     a b p? jz[:end-of-then]
    //     c d
    //   :end-of-then

    //// if then else
    //   (if a b p? then c d else e f)
    //// ==>
    //     a b p? jz[:end-of-then]
    //     c d jmp[:end-of-else]
    //   :end-of-then
    //     e f
    //   :end-of-else

    void k_if() {
      compile_until_meet_jo(JO_THEN);
      emit(JO_INS_JZ);
      cell* end_of_then = tos(compiling_stack);
      p_compiling_stack_inc();
      jo_t ending_jo = compile_until_meet_jo_or_jo(JO_ELSE, ROUND_KET);
      if (ending_jo == ROUND_KET) {
        end_of_then[0] = (cell*)tos(compiling_stack) - end_of_then;
        return;
      }
      else {
        emit(JO_INS_JMP);
        cell* end_of_else = tos(compiling_stack);
        p_compiling_stack_inc();
        end_of_then[0] = (cell*)tos(compiling_stack) - end_of_then;
        p_compile_until_round_ket();
        end_of_else[0] = (cell*)tos(compiling_stack) - end_of_else;
        return;
      }
    }
    void compile_maybe_square() {
      jo_t first_jo = read_jo();
      if (first_jo == SQUARE_BAR) { compile_until_meet_jo(SQUARE_KET); }
      else { compile_jo(first_jo); }
    }
    //   (case [...]
    //     tag [...]
    //     ...)
    //// ==>
    //     [...]
    //     dup tag 'tag eq? jz[:end-of-this-case]
    //     drop [...] jmp[:end-of-case]
    //   :end-of-this-case
    //     ... ...
    //   :end-of-case
    //     drop

    void k_case() {
      compile_maybe_square();
      cell counter = 0;
      cell case_ends[256];

      while (true) {
        jo_t dc = read_jo();
        if (dc == ROUND_KET) { break; }

        emit(str2jo("dup"));
        emit(str2jo("tag"));
        {
          char* tmp = malloc(strlen(jo2str(dc) + 1));
          tmp[0] = '\0';
          strcat(tmp, jo2str(dc));
          emit(JO_INS_LIT); emit(TAG_JO); emit(str2jo(tmp));
          free(tmp);
        }
        emit(str2jo("eq?"));

        emit(JO_INS_JZ);
        jo_t* end_of_this_case = tos(compiling_stack);
        p_compiling_stack_inc();
        emit(str2jo("drop"));
        compile_maybe_square();

        emit(JO_INS_JMP);
        case_ends[counter] = tos(compiling_stack);
        counter++;
        p_compiling_stack_inc();

        end_of_this_case[0] = (jo_t*)tos(compiling_stack) - end_of_this_case;
      }

      while (counter > 0) {
        counter--;
        jo_t* end_of_case = case_ends[counter];
        end_of_case[0] = (jo_t*)tos(compiling_stack) - end_of_case;
      }
    }
    //   (cond
    //     [:t1 leaf? :t2 leaf? and] [...]
    //     [:t1 node? :t2 node? and] [...]
    //     else [else-body])
    //// ==>
    //     [:t1 leaf? :t2 leaf? and] jz[:end-of-this-cond]
    //     [...] jmp[:end-of-cond]
    //   :end-of-this-cond
    //     [:t1 node? :t2 node? and] jz[:end-of-this-cond]
    //     [...] jmp[:end-of-cond]
    //   :end-of-this-cond
    //     [else-body]
    //   :end-of-cond

    void k_cond() {
      cell counter = 0;
      cell cond_ends[256];
      while (true) {
        jo_t s = read_jo();
        if (s == ROUND_KET) { break; }
        else if (s == JO_ELSE) {
          compile_maybe_square();
          k_ignore();
          break;
        }
        jo_unread(s);
        compile_maybe_square();
        emit(JO_INS_JZ);
        cell* end_of_this_cond = tos(compiling_stack);
        p_compiling_stack_inc();

        compile_maybe_square();
        emit(JO_INS_JMP);
        cond_ends[counter] = tos(compiling_stack);
        counter++;
        p_compiling_stack_inc();

        end_of_this_cond[0] = (cell*)tos(compiling_stack) - end_of_this_cond;
      }
      while (counter > 0) {
        counter--;
        cell* end_of_cond = cond_ends[counter];
        end_of_cond[0] = (cell*)tos(compiling_stack) - end_of_cond;
      }
    }
    void p_recur() {
      struct rp p = rs_tos();
      ds_push(p.t, p.d);
      disp_exe(JO_EXE->data, p.t);
    }
    void expose_control() {
      plus_prim("note", k_ignore);
      plus_prim("ins/lit", ins_lit);

      plus_prim("ins/jmp", ins_jmp);
      plus_prim("ins/jz", ins_jz);

      plus_prim("if", k_if);
      plus_prim("begin", p_compile_until_round_ket);

      plus_prim("case", k_case);
      plus_prim("cond", k_cond);

      plus_prim("recur", p_recur);
    }
    void k_run() {
      jo_t* jojo = tos(compiling_stack);

      {
        compile_until_meet_jo(ROUND_KET);
        emit(JO_END);
        emit(0);
        emit(0);
      }

      rs_push(jojo, TAG_JOJO, jojo, current_local_counter);
      eval();
    }
    bool test_flag = false;
    void p_test_flag() { ds_push(TAG_BOOL, test_flag); }
    void p_test_flag_on() { test_flag = true; }
    void p_test_flag_off() { test_flag = false; }

    void k_test() {
      if (test_flag) {
        k_run();
      }
    }
    void k_plus_var() {
      jo_t name = read_jo();
      k_run();
      struct dp a = ds_pop();
      bind_name(name, a.t, a.d);

      char name_buffer[1024];
      name_buffer[0] = '\0';
      strcat(name_buffer, jo2str(name));
      strcat(name_buffer, "!");
      bind_name(str2jo(name_buffer), str2jo("<set-global-variable>"), name);
    }
    #define MAX_FIELDS 1024

    void k_plus_data() {
      jo_t name = read_jo();
      jo_t fields[MAX_FIELDS];
      cell i = 0;
      while (true) {
        if (i >= MAX_FIELDS) {
          k_ignore();
          report("- k_plus_data fail\n");
          report("  too many fields\n");
          report("  MAX_FIELDS : %ld\n", MAX_FIELDS);
          return;
        }
        jo_t field = read_jo();
        if (field == ROUND_KET) {
          fields[i] = 0;
          i++;
          break;
        }
        fields[i] = field;
        i++;
      }
      jo_t* fresh_fields = (jo_t*)malloc(i*sizeof(jo_t));
      while (i > 0) {
        i--;
        fresh_fields[i] = fields[i];
      }
      _plus_data(name, fresh_fields);
    }
    void k_arrow() {
      jo_t jo = read_jo();
      if (jo == str2jo("--")) {
        k_ignore();
        return;
      }
      else if (jo == ROUND_KET) {
        return;
      }
      else if (get_local_string_p(jo2str(jo))) {
        k_arrow();
        emit(JO_INS_SET_LOCAL);
        emit(jo);
      }
      else {
        k_arrow();
      }
    }
    void k_plus_jojo() {
      jo_t fun_name = read_jo();
      jo_t* jojo = tos(compiling_stack);

      {
        compile_until_meet_jo(ROUND_KET);
        emit(JO_END);
        emit(0);
        emit(0);
      }

      bind_name(fun_name, TAG_JOJO, jojo);
    }
    cell k_plus_gene_count_arity_from_type() {
      cell arity = 0;
      while (true) {
        jo_t jo = read_jo();
        if (jo == str2jo("--")) {
          k_ignore();
          break;
        }
        else if (jo == ROUND_KET) {
          break;
        }
        arity++;
      }
      return arity;
    }
    void k_plus_gene() {
      jo_t gene_name = read_jo();
      read_jo(); // drop '('
      read_jo(); // drop '->'
      cell arity = k_plus_gene_count_arity_from_type();
      k_ignore();
      plus_gene(jo2str(gene_name), arity);
    }
    void k_plus_disp_collect_tags_from_type(jo_t* tags) {
      jo_t jo = read_jo();
      if (jo == str2jo("--")) {
        k_ignore();
        tags[0] = 0;
        return;
      }
      else if (jo == ROUND_KET) {
        tags[0] = 0;
        return;
      }
      else if (get_local_string_p(jo2str(jo))) {
        k_plus_disp_collect_tags_from_type(tags);
        emit(JO_INS_SET_LOCAL);
        emit(jo);
      }
      else if (tag_string_p(jo2str(jo))) {
        tags[0] = jo;
        k_plus_disp_collect_tags_from_type(tags+1);
      }
      else {
        k_plus_disp_collect_tags_from_type(tags);
      }
    }
    void k_plus_disp() {
      jo_t gene_name = read_jo();
      jo_t tags[16];
      read_jo(); // drop '('
      read_jo(); // drop '->'

      k_plus_disp_collect_tags_from_type(tags);

      jo_t* jojo = tos(compiling_stack);
      {
        compile_until_meet_jo(ROUND_KET);
        emit(JO_END);
        emit(0);
        emit(0);
      }

      _plus_disp(gene_name, tags, TAG_JOJO, jojo);
    }
    void expose_top() {
      plus_prim("run", k_run);

      plus_prim("test", k_test);
      plus_prim("test-flag", p_test_flag);
      plus_prim("test-flag-on", p_test_flag_on);
      plus_prim("test-flag-off", p_test_flag_off);

      plus_prim("+var", k_plus_var);
      plus_prim("->", k_arrow);
      plus_prim("+jojo", k_plus_jojo);
      plus_prim("+data", k_plus_data);
      plus_prim("+gene", k_plus_gene);
      plus_prim("+disp", k_plus_disp);
    }
    void data_print(jo_t tag, cell data);

    void local_env_print(struct local* lr) {
      report("{ ");
      while (lr->name != 0) {
        data_print(lr->local_tag, lr->local_data);
        report("%s! ", jo2str(lr->name));
        lr++;
      }
      report("}");
    }
    void jojo_print(jo_t* jojo);

    void data_print(jo_t tag, cell data) {
      if (tag == TAG_INT) {
        report("%ld ", data);
      }
      else if (tag == TAG_STRING) {
        struct gp* gp = data;
        char* str = gp->p;
        report("\"%s\" ", str);
      }
      else if (tag == TAG_JO) {
        jo_t jo = data;
        report("'%s ", jo2str(jo));
      }
      else if (tag == TAG_BOOL) {
        if (data) {
          report("true ");
        }
        else {
          report("flase ");
        }
      }
      else if (tag == TAG_JOJO) {
        jo_t* jojo = data;
        jojo_print(jojo);
      }
      else if (tag == str2jo("<local-env>")) {
        struct local* lr = data;
        local_env_print(lr);
        report("<local-env> ");
      }
      else if (tag == TAG_CLOSURE) {
        struct gp* closure = data;

        struct dp a = get_field(TAG_CLOSURE, closure, str2jo(".local-env"));
        struct gp* ao = a.d;
        struct local* lr = ao->p;

        struct dp b = get_field(TAG_CLOSURE, closure, str2jo(".jojo"));
        jo_t* jojo = b.d;

        local_env_print(lr);
        report("+");
        jojo_print(jojo);
      }
      else {
        report("%s ", jo2str(tag));
      }
    }
    void jojo_print(jo_t* jojo) {
      report("[ ");
      while (true) {
        if (jojo[0] == 0 && jojo[1] == 0) {
          break;
        }
        else if (jojo[0] == JO_INS_LIT) {
          data_print(jojo[1], jojo[2]);
          jojo++;
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_JZ) {
          report("(jz %ld) ", jojo[1]);
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_JMP) {
          report("(jmp %ld) ", jojo[1]);
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_LOOP) {
          report("(loop) ");
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_RECUR) {
          report("(recur) ");
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_GET_LOCAL ||
                 jojo[0] == JO_INS_GET_FIELD) {
          report("%s ", jo2str(jojo[1]));
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_SET_LOCAL ||
                 jojo[0] == JO_INS_SET_FIELD) {
          report("%s! ", jo2str(jojo[1]));
          jojo++;
          jojo++;
        }
        else if (jojo[0] == JO_INS_TAIL_CALL) {
          report("(tail-call %s) ", jo2str(jojo[1]));
          jojo++;
          jojo++;
        }
        else {
          report("%s ", jo2str(jojo[0]));
          jojo++;
        }
      }
      report("] ");
    }
    void p_print_ds() {
      cell length = stack_length(ds);
      report("  * %ld *  ", length/2);
      report("-- ");
      cell cursor = 0;
      while (cursor < length) {
        data_print(stack_ref(ds, cursor+1),
                   stack_ref(ds, cursor));
        cursor++;
        cursor++;
      }
      report("--\n");
    }
    void print_return_point(struct rp p) {
      jo_t* jojo = p.j;
      report("    - { %s } ", jo2str(*(jojo - 1)));
      jojo_print(jojo);
      report("\n");
    }
    void p_print_rs() {
      cell length = rs_length();
      report("  - return-stack * %ld * :\n", length);
      if (length == 0) { return; };
      cell index = 0;
      while (index < length - 1) {
        struct rp p = rs_ref(index);
        print_return_point(p);
        index++;
      }
      { // tos of rs is special
        struct rp p = rs_ref(index);
        jo_t* jojo = p.j;
        report("    - ");
        jojo_print(jojo);
        report("\n");
        index++;
        index++;
      }
    }
    bool repl_flag = false;
    void p_repl_flag() { ds_push(TAG_BOOL, repl_flag); }
    void p_repl_flag_on() { repl_flag = true; }
    void p_repl_flag_off() { repl_flag = false; }

    void repl(struct input_stack* input_stack) {
      push(reading_stack, input_stack);
      while (true) {
        if (!has_jo_p()) {
          return;
        }
        jo_t s = read_jo();
        if (s == ROUND_BAR) {
          jo_apply(read_jo());
          if (repl_flag) {
            p_print_ds();
          }
        }
        else {
          // loop
        }
      }
      drop(reading_stack);
      input_stack_free(input_stack);
    }
    cell debug_repl_level = 0;

    void debug_repl(struct input_stack* input_stack) {
      push(reading_stack, input_stack);
      while (true) {
        if (!has_jo_p()) {
          return;
        }
        jo_t jo = read_jo();
        if (jo == str2jo("help")) {
          report("- debug-repl usage :\n");
          report("  - available commands :\n");
          report("    help exit bye\n");
        }
        else if (jo == str2jo("exit")) {
          return;
        }
        else if (jo == str2jo("bye")) {
          p_bye();
          return;
        }
        else if (jo == ROUND_BAR) {
          jo_apply(read_jo());
          p_print_ds();
          report("debug[%ld]> ", debug_repl_level);
        }
        else {
          // loop
        }
      }
      drop(reading_stack);
      input_stack_free(input_stack);
    }
    void p_debug() {
      report("- in debug-repl [level %ld] >_<!\n", debug_repl_level);
      p_print_rs();
      p_print_ds();
      report("debug[%ld]> ", debug_repl_level);
      debug_repl_level++;
      debug_repl(terminal_input_stack());
      debug_repl_level--;
      report("- exit debug-repl [level %ld]\n", debug_repl_level);
    }
      void kernel_signal_handler(int sig, siginfo_t *siginfo, void *ucontext) {
        fflush(stdin);
        fflush(stdout);
        fflush(stderr);

        report("- kernel_signal_handler\n");
        psiginfo(siginfo, "  signal ");

        int errno_backup;
        errno_backup = errno;

        p_debug();

        errno = errno_backup;
      }
      void init_kernel_signal_handler() {
        struct sigaction kernel_signal_action;

        sigemptyset(&kernel_signal_action.sa_mask);

        kernel_signal_action.sa_flags = SA_SIGINFO | SA_NODEFER | SA_RESTART;
        kernel_signal_action.sa_sigaction = kernel_signal_handler;

        int sig_array[] = { SIGSEGV, SIGBUS, SIGFPE, SIGILL,
                            SIGPIPE, SIGSYS, SIGXCPU, SIGXFSZ};
        int sig_array_length = sizeof(sig_array)/sizeof(sig_array[0]);
        cell i = 0;
        while (i < sig_array_length) {
          if (sigaction(sig_array[i], &kernel_signal_action, 0) == -1) {
            perror("- init_kernel_signal_handler fail");
          }
          i++;
        }
      }
    void expose_repl() {
      plus_prim("repl-flag", p_repl_flag);
      plus_prim("repl-flag-on", p_repl_flag_on);
      plus_prim("repl-flag-off", p_repl_flag_off);

      plus_prim("debug", p_debug);
    }
    bool step_flag = false;
    cell stepper_counter = 0;
    cell pending_steps = 0;

    void report_one_step() {
      while (true) {
        if (pending_steps > 0) {
          p_print_rs();
          p_print_ds();
          stepper_counter++;
          report("- stepper counting : %ld\n", stepper_counter);
          pending_steps--;
          return;
        }
        if (!has_jo_p()) {
          step_flag = false;
          return;
        }
        jo_t jo = read_jo();
        if (jo == str2jo("help")) {
          report("- stepper usage :\n");
          report("  type '.' to execute one step\n");
          report("  type a numebr to execute the number of steps\n");
          report("  - available commands :\n");
          report("    help exit bye\n");
        }
        else if (jo == str2jo(".")) {
          p_print_rs();
          p_print_ds();
          stepper_counter++;
          report("- stepper counting : %ld\n", stepper_counter);
          return;
        }
        else if (nat_string_p(jo2str(jo))) {
          p_print_rs();
          p_print_ds();
          stepper_counter++;
          report("- stepper counting : %ld\n", stepper_counter);
          pending_steps = string_to_int(jo2str(jo)) - 1;
          return;
        }
        else if (jo == str2jo("exit")) {
          step_flag = false;
          return;
        }
        else if (jo == str2jo("bye")) {
          p_bye();
          return;
        }
        else {
          // loop
        }
      }
    }
    // do not handle tail call
    void p_step() {
      struct input_stack* input_stack = terminal_input_stack();
      push(reading_stack, input_stack);
      step_flag = true;
      stepper_counter = 0;
      pending_steps = 0;
      report("stepper> ");
      cell base = rs->pointer;
      while (rs->pointer >= base) {
        if (step_flag == false) { break; };
        struct rp p = rs_tos();
        rs_inc();
        jo_t* jojo = p.j;
        jo_t jo = jojo[0];
        jo_apply(jo);
        {
          report_one_step();
        }
      }
      if (rs->pointer >= base) {
        report("- exit stepper\n");
      }
      if (rs->pointer < base) {
        report("- the stepped jojo is finished\n");
        report("- automatically exit stepper\n");
      }
      drop(reading_stack);
      input_stack_free(input_stack);
    }
    void expose_step() {
      plus_prim("step", p_step);
    }
    void p_true() {
      ds_push(TAG_BOOL, true);
    }
    void p_false() {
      ds_push(TAG_BOOL, false);
    }
    void p_not() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, !a.d);
    }
    void p_and() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, a.d && b.d);
    }
    void p_or() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, a.d || b.d);
    }
    void expose_bool() {
      plus_atom("<bool>", gc_ignore);

      plus_prim("true", p_true);
      plus_prim("false", p_false);
      plus_prim("not", p_not);
      plus_prim("and", p_and);
      plus_prim("or", p_or);
    }
    void string_write(char* str) {
      while (str[0] != '\0') {
        byte_write(str[0]);
        str++;
      }
    }
    void p_string_write() {
      struct dp a = ds_pop();
      struct gp* ao = a.d;
      string_write(ao->p);
    }
    void p_string_len() {
      struct dp a = ds_pop();
      struct gp* ao = a.d;
      ds_push(TAG_INT, strlen(ao->p));
    }
    void p_string_ref() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      struct gp* bo = b.d;
      char* str = bo->p;
      ds_push(TAG_BYTE, str[a.d]);
    }
    void p_string_cat() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      struct gp* ao = a.d;
      struct gp* bo = b.d;
      char* str0 = bo->p;
      char* str1 = ao->p;

      char* str2 = (char*)malloc(strlen(str0) + strlen(str1) + 1);
      str2[0] = '\0';
      strcat(str2, str0);
      strcat(str2, str1);

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_free;
      gp->p = str2;

      ds_push(TAG_STRING, gp);
    }
    void p_string_slice() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      struct dp c = ds_pop();
      struct gp* co = c.d;
      char* str0 = co->p;
      cell begin = b.d;
      cell end = a.d;
      char* str1 = substring(str0, begin, end);

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_free;
      gp->p = str1;

      ds_push(TAG_STRING, gp);
    }
    void p_string_empty_p() {
      struct dp a = ds_pop();
      struct gp* ao = a.d;
      char* str = ao->p;
      ds_push(TAG_BOOL, str[0] == '\0');
    }
    void p_string_eq_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      struct gp* ao = a.d;
      struct gp* bo = b.d;
      ds_push(TAG_BOOL, string_equal(ao->p, ao->p));
    }
    void p_read_string() {
      char buffer[1024 * 1024];
      cell cursor = 0;
      while (true) {
        char c = read_byte();
        if (c == '"') {
          buffer[cursor] = '\0';
          cursor++;
          break;
        }
        else {
          buffer[cursor] = c;
          cursor++;
        }
      }
      char* str = strdup(buffer);
      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_ignore;
      gp->p = str;

      ds_push(TAG_STRING, gp);
    }
    void expose_string() {
      plus_prim("string-write", p_string_write);
      plus_prim("string-len", p_string_len);
      plus_prim("string-ref", p_string_ref);
      plus_prim("string-cat", p_string_cat);
      plus_prim("string-slice", p_string_slice);
      plus_prim("string-empty?", p_string_empty_p);
      plus_prim("string-eq?", p_string_eq_p);
      plus_prim("read-string", p_read_string);
    }
    void p_inc() {
      struct dp a = ds_pop();
      ds_push(TAG_INT, a.d + 1);
    }
    void p_dec() {
      struct dp a = ds_pop();
      ds_push(TAG_INT, a.d - 1);
    }
    void p_neg() {
      struct dp a = ds_pop();
      ds_push(TAG_INT, - a.d);
    }
    void p_add() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_INT, a.d + b.d);
    }
    void p_sub() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_INT, b.d - a.d);
    }
    void p_mul() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_INT, a.d * b.d);
    }
    void p_div() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_INT, b.d / a.d);
    }
    void p_mod() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_INT, b.d % a.d);
    }
    void p_gt_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, b.d > a.d);
    }
    void p_lt_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, b.d < a.d);
    }
    void p_gteq_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, b.d >= a.d);
    }
    void p_lteq_p() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      ds_push(TAG_BOOL, b.d <= a.d);
    }
    void p_int_write() {
      char buffer [32];
      struct dp a = ds_pop();
      sprintf(buffer, "%ld", a.d);
      string_write(buffer);
    }
    void expose_int() {
      plus_prim("inc", p_inc);
      plus_prim("dec", p_dec);
      plus_prim("neg", p_neg);

      plus_prim("add", p_add);
      plus_prim("sub", p_sub);

      plus_prim("mul", p_mul);
      plus_prim("div", p_div);
      plus_prim("mod", p_mod);

      plus_prim("gt?", p_gt_p);
      plus_prim("lt?", p_lt_p);
      plus_prim("gteq?", p_gteq_p);
      plus_prim("lteq?", p_lteq_p);

      plus_prim("int-write", p_int_write);
    }
    void p_round_bar()    { ds_push(TAG_JO, ROUND_BAR); }
    void p_round_ket()    { ds_push(TAG_JO, ROUND_KET); }
    void p_square_bar()   { ds_push(TAG_JO, SQUARE_BAR); }
    void p_square_ket()   { ds_push(TAG_JO, SQUARE_KET); }
    void p_flower_bar()   { ds_push(TAG_JO, FLOWER_BAR); }
    void p_flower_ket()   { ds_push(TAG_JO, FLOWER_KET); }

    void p_doublequote()  { ds_push(TAG_JO, DOUBLEQUOTE); }
    void p_singlequote()  { ds_push(TAG_JO, SINGLEQUOTE); }
    void p_quasiquote()   { ds_push(TAG_JO, QUASIQUOTE); }
    void p_comma()        { ds_push(TAG_JO, COMMA); }
    void p_jo_write() {
      struct dp a = ds_pop();
      string_write(jo2str(a.d));
    }
    void p_jo_unread() {
      struct dp a = ds_pop();
      jo_unread(a.d);
    }
    void p_jo_apply() {
      struct dp a = ds_pop();
      jo_apply(a.d);
    }
    void p_jo_to_int() {
      struct dp a = ds_pop();
      ds_push(TAG_INT, string_to_int(jo2str(a.d)));
    }
    void p_int_jo_p() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, int_string_p(jo2str(a.d)));
    }
    void p_get_local_jo_p() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, get_local_string_p(jo2str(a.d)));
    }
    void p_set_local_jo_p() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, set_local_string_p(jo2str(a.d)));
    }
    void p_get_field_jo_p() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, get_field_string_p(jo2str(a.d)));
    }
    void p_set_field_jo_p() {
      struct dp a = ds_pop();
      ds_push(TAG_BOOL, set_field_string_p(jo2str(a.d)));
    }
    void expose_jo() {
      plus_prim("round-bar",    p_round_bar);
      plus_prim("round-ket",    p_round_ket);
      plus_prim("square-bar",   p_square_bar);
      plus_prim("square-ket",   p_square_ket);
      plus_prim("flower-bar",   p_flower_bar);
      plus_prim("flower-ket",   p_flower_ket);

      plus_prim("doublequote",  p_doublequote);
      plus_prim("singlequote",  p_singlequote);
      plus_prim("quasiquote",   p_quasiquote);
      plus_prim("comma",        p_comma);

      plus_prim("jo-write",  p_jo_write);
      plus_prim("jo-unread", p_jo_unread);
      plus_prim("jo-apply",  p_jo_apply);
      plus_prim("jo->int",   p_jo_to_int);

      plus_prim("int-jo?",       p_int_jo_p);
      plus_prim("get-local-jo?", p_get_local_jo_p);
      plus_prim("set-local-jo?", p_set_local_jo_p);
      plus_prim("get-field-jo?", p_get_field_jo_p);
      plus_prim("set-field-jo?", p_set_field_jo_p);
    }
    void p_compiling_stack_tos() {
      ds_push(TAG_ADDRESS, tos(compiling_stack));
    }
    void p_set_offset_to_here() {
      struct dp a = ds_pop();
      cell* address = a.d;
      address[0] = (cell*)tos(compiling_stack) - address;
    }
    void expose_address() {
      plus_prim("compiling-stack-tos", p_compiling_stack_tos);
      plus_prim("set-offset-to-here", p_set_offset_to_here);
    }
    void p_compiling_stack_tos_as_jojo() {
      ds_push(TAG_JOJO, tos(compiling_stack));
    }
    void expose_jojo() {
      plus_prim("compiling-stack-tos-as-jojo",
               p_compiling_stack_tos_as_jojo);
    }
    void gc_local_env(gc_state_t gc_state, struct gp* gp) {
      if (gc_state == GC_STATE_MARKING) {
        if (gp->mark == GC_MARK_USING) { return; }
        gp->mark = GC_MARK_USING;
        struct local* lr = gp->p;
        while (lr->name != 0) {
            mark_one(lr->local_tag, lr->local_data);
          lr++;
        }
      }
      else if (gc_state == GC_STATE_SWEEPING) {
        free(gp->p);
      }
    }
    // caller free
    struct local* current_local_record() {
      struct rp p = rs_tos();
      cell length = current_local_counter - p.l;
      cell i = 0;
      struct local* lr = (struct local*)
        malloc((length + 1) * sizeof(struct local));
      while (i < length) {
        lr[i].name       = local_record[p.l + i].name;
        lr[i].local_tag  = local_record[p.l + i].local_tag;
        lr[i].local_data = local_record[p.l + i].local_data;
        i++;
      }
      lr[i].name = 0;
      return lr;
    }
    void p_current_local_env() {
      struct local* lr = current_local_record();
      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_local_env;
      gp->p = lr;
      ds_push(str2jo("<local-env>"), gp);
    }
    void set_local_record(struct local* lr) {
      while (lr->name != 0) {
        set_local(lr->name, lr->local_tag, lr->local_data);
        lr++;
      }
    }
    void p_closure_exe() {
      struct dp c = ds_pop();
      struct gp* closure = c.d;

      struct dp a = get_field(TAG_CLOSURE, closure, str2jo(".local-env"));
      struct gp* ao = a.d;
      struct local* lr = ao->p;

      struct dp b = get_field(TAG_CLOSURE, closure, str2jo(".jojo"));
      jo_t* jojo = b.d;

      cell local_counter = current_local_counter;
      set_local_record(lr);
      rs_push(jojo, TAG_CLOSURE, closure, local_counter);
    }
    void k_closure() {
      jo_t* jojo = tos(compiling_stack);

      {
        compile_until_meet_jo(FLOWER_KET);
        emit(JO_END);
        emit(0);
        emit(0);
      }

      jo_t* new_jojo =
        array_len_dup(jojo, (cell*)tos(compiling_stack) - (cell*)jojo);
      drop(compiling_stack);
      push(compiling_stack, jojo);

      emit(JO_INS_LIT);
      emit(TAG_JOJO);
      emit(new_jojo);

      emit(JO_CURRENT_LOCAL_ENV);
      emit(JO_CLOSURE);
    }
    void expose_closure() {
      plus_prim("current-local-env", p_current_local_env);
      plus_atom("<local-env>", gc_local_env);

      plus_data("<closure>", J(".jojo", ".local-env"));
      plus_prim("closure-exe", p_closure_exe);
      plus_disp("exe", J("<closure>"), "<prim>", p_closure_exe);

      plus_gene("apply", 1);
      plus_disp("apply", J("<closure>"), "<prim>", p_closure_exe);
      plus_disp("apply", J("<jojo>"), "<prim>", p_jojo_exe);
      plus_prim("jojo-apply", p_jojo_exe);
      plus_prim("closure-apply", p_closure_exe);
    }
    void p_tcp_socket_listen() {
      // [:service <string> :backlog <int>] -> [<socket>]

      struct addrinfo hints, *servinfo, *p;
      int yes = 1;

      struct dp a = ds_pop();
      int backlog = a.d;

      struct dp b = ds_pop();
      struct gp* bo = b.d;
      char* service = bo->p;

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;

      int rv = getaddrinfo(0, service, &hints, &servinfo);
      if (rv != 0) {
        report("- p_tcp_socket_listen fail to getaddrinfo\n");
        report("  service : %s\n", service);
        report("getaddrinfo: %s\n", gai_strerror(rv));
        p_debug();
        return;
      }

      int sockfd;
      for(p = servinfo; p != 0; p = p->ai_next) {
        sockfd = socket(p->ai_family,
                        p->ai_socktype,
                        p->ai_protocol);
        if (sockfd == -1) { continue; }
        // ><><>< why setsockopt ?
        if (setsockopt(sockfd,
                       SOL_SOCKET,
                       SO_REUSEADDR, &yes, sizeof(int)) == -1) {
          report("- p_tcp_socket_listen fail to listen\n");
          report("  service : %s\n", service);
          perror("  setsockopt error : ");
          p_debug();
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          continue;
        }
        break;
      }
      freeaddrinfo(servinfo);

      if (p == 0)  {
        report("- p_tcp_socket_listen fail to bind\n");
        report("  service : %s\n", service);
        p_debug();
      }

      if (listen(sockfd, backlog) == -1) {
        report("- p_tcp_socket_listen fail to listen\n");
        report("  service : %s\n", service);
        perror("  listen error : ");
        p_debug();
      }

      ds_push(str2jo("<socket>"), sockfd);
    }
    // get sockaddr, ipv4 or ipv6:
    void *get_in_addr(struct sockaddr *sa) {
      if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
      }
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
    void p_socket_accept() {
      // [:sockfd <socket>] ->
      // [:newfd <socket> :connector-address <string>]

      struct dp a = ds_pop();
      int sockfd = a.d;

      struct sockaddr_storage their_addr; // connector's address information
      socklen_t sin_size;
      char str[INET6_ADDRSTRLEN];

      sin_size = sizeof their_addr;
      int newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
      if (newfd == -1) {
        report("- p_socket_accept fail\n");
        perror("  accept error : ");
        return;
      }

      inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                str,
                sizeof(str));

      ds_push(str2jo("<socket>"), newfd);

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_free;
      gp->p = strdup(str);

      ds_push(TAG_STRING, gp);
    }
    void p_tcp_socket_connect() {
      // [:host <string> :service <string>] -> [<socket>]

      struct dp a = ds_pop();
      struct gp* ao = a.d;
      char* service = ao->p;

      struct dp b = ds_pop();
      struct gp* bo = b.d;
      char* host = bo->p;

      struct addrinfo hints, *servinfo, *p;

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;

      int rv = getaddrinfo(host, service, &hints, &servinfo);
      if (rv != 0) {
        report("- p_tcp_socket_connect fail to getaddrinfo\n");
        report("  host : %s\n", host);
        report("  service : %s\n", service);
        report("  getaddrinfo error : %s\n", gai_strerror(rv));
        p_debug();
        return;
      }

      int sockfd;
      for(p = servinfo; p != 0; p = p->ai_next) {
        sockfd = socket(p->ai_family,
                        p->ai_socktype,
                        p->ai_protocol);
        if (sockfd == -1) { continue; }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          continue;
        }
        break;
      }
      freeaddrinfo(servinfo);

      if (p == 0)  {
        report("- p_tcp_socket_connect fail to connect\n");
        report("  host : %s\n", host);
        report("  service : %s\n", service);
        p_debug();
      }

      ds_push(str2jo("<socket>"), sockfd);
    }
    void p_socket_send() {
      // [<socket> <string>] -> []

      struct dp a = ds_pop();
      struct gp* ao = a.d;
      char* str = ao->p;

      struct dp b = ds_pop();
      int sockfd = b.d;

      if (send(sockfd, str, strlen(str), 0) == -1) {
        report("- p_socket_send fail\n");
        perror("  send error : ");
      }
    }
    void p_socket_recv() {
      // [<socket>] -> [<string>]
      struct dp a = ds_pop();
      int sockfd = a.d;

      char* buf[1024];

      ssize_t real_bytes = recv(sockfd, buf, 1024-1, 0);
      if (real_bytes == -1) {
        report("- p_socket_recv fail\n");
        perror("  recv error : ");
      }

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_free;
      gp->p = strdup(buf);

      ds_push(TAG_STRING, gp);
    }
    void p_close() {
      // [:sockfd <socket>] -> []
      struct dp a = ds_pop();
      int sockfd = a.d;
      if (close(sockfd) == -1) {
        report("- p_close fail\n");
        perror("  close error : ");
      };
    }
    void expose_socket() {
      plus_atom("<socket>", gc_ignore);
      plus_prim("tcp-socket-listen", p_tcp_socket_listen);
      plus_prim("socket-accept", p_socket_accept);
      plus_prim("tcp-socket-connect", p_tcp_socket_connect);
      plus_prim("socket-send", p_socket_send);
      plus_prim("socket-recv", p_socket_recv);
      plus_prim("close", p_close);
    }
    cell cmd_number;

    void p_cmd_number() {
      ds_push(TAG_INT, cmd_number);
    }
    char** cmd_string_array;

    void p_index_to_cmd_string() {
      // index -> string
      struct dp a = ds_pop();
      cell index = a.d;
      char* cmd_string = cmd_string_array[index];

      struct gp* gp = new_record_gp();
      gp->gc_actor = gc_free;
      gp->p = strdup(cmd_string);

      ds_push(TAG_STRING, gp);
    }
    void p_find_env_string() {
      // string -> [env-string true] or [false]
      struct dp a = ds_pop();
      struct gp* ao = a.d;
      char* var_string = ao->p;
      char* env_string = getenv(var_string);
      if (env_string == 0) {
        ds_push(TAG_BOOL, false);
      }
      else {
        struct gp* gp = new_record_gp();
        gp->gc_actor = gc_free;
        gp->p = strdup(env_string);

        ds_push(TAG_STRING, gp);
        ds_push(TAG_BOOL, true);
      }
    }
    void expose_system() {
      plus_prim("cmd-number", p_cmd_number);
      plus_prim("index->cmd-string", p_index_to_cmd_string);
      plus_prim("find-env-string", p_find_env_string);
    }
    void ccall (char* function_name, void* lib) {
      primitive_t fun = dlsym(lib, function_name);
      if (fun == 0) {
        report("- ccall fail\n");
        report("  function_name : %s\n", function_name);
        report("  dynamic link error : %s\n", dlerror());
      };
      fun();
    }
    void erase_real_path_to_dir(char* path) {
      cell cursor = strlen(path);
      while (path[cursor] != '/') {
        path[cursor] = '\0';
        cursor--;
      }
      path[cursor] = '\0';
    }

    char* get_real_reading_path(char* path) {
      // caller of this function
      // should free its return value
      char* real_reading_path = malloc(PATH_MAX);
      if (path[0] == '/' ||
          ((struct input_stack*)tos(reading_stack))->type == INPUT_STACK_TERMINAL) {
        realpath(path, real_reading_path);
        return real_reading_path;
      }
      else {
        char* proc_link_path = malloc(PATH_MAX);
        sprintf(proc_link_path,
                "/proc/self/fd/%d",
                ((struct input_stack*)tos(reading_stack))->file);
        ssize_t real_bytes = readlink(proc_link_path, real_reading_path, PATH_MAX);
        if (real_bytes == -1) {
          report("- get_real_reading_path fail to readlink\n");
          report("  proc_link_path : %s\n", proc_link_path);
          perror("  readlink : ");
          free(proc_link_path);
          free(real_reading_path);
          p_debug();
        }
        free(proc_link_path);
        real_reading_path[real_bytes] = '\0';
        erase_real_path_to_dir(real_reading_path);
        strcat(real_reading_path, "/");
        strcat(real_reading_path, path);
        return real_reading_path;
      }
    }
    void k_clib_one() {
      // "..."
      char* path = malloc(PATH_MAX);
      cell cursor = 0;
      while (true) {
        char c = read_byte();
        if (c == '"') {
          path[cursor] = 0;
          cursor++;
          break;
        }
        else {
          path[cursor] = c;
          cursor++;
        }
      }
      char* real_read_path = get_real_reading_path(path);
      free(path);
      void* lib = dlopen(real_read_path, RTLD_LAZY);
      if (lib == 0) {
        report("- k_clib_one fail to open library\n");
        report("  real_read_path : %s\n", real_read_path);
        report("  dynamic link error : %s\n", dlerror());
        p_debug();
        return;
      };
      free(real_read_path);
      ccall("expose", lib);
    }
    void k_clib() {
      while (true) {
        jo_t s = read_jo();
        if (s == ROUND_KET) {
          return;
        }
        else if (s == DOUBLEQUOTE) {
          k_clib_one();
        }
        else {
          // do nothing
        }
      }
    }
    void expose_cffi() {
      plus_prim("+clib", k_clib);
    }
    void p_core_flag() { ds_push(TAG_BOOL, core_flag); }
    void p_core_flag_on() { core_flag = true; }
    void p_core_flag_off() { core_flag = false; }
    // void load_core() {
    //   #include "core.h"
    //   core_jo[core_jo_len - 1] = '\0';
    //   repl(string_input_stack((char*)core_jo));
    // }
    void p_bind_name() {
      struct dp a = ds_pop();
      struct dp b = ds_pop();
      bind_name(b.d, a.t, a.d);
    }
    void p_jo_emit_call() {
      struct dp a = ds_pop();
      emit(a.d);
    }
    void p_int_emit_data() {
      struct dp a = ds_pop();
      emit(a.d);
    }
    void p_emit_lit() {
      struct dp a = ds_pop();
      emit(JO_INS_LIT);
      emit(a.t);
      emit(a.d);
    }
    void p_jo_emit_get_local() {
      struct dp a = ds_pop();
      emit(JO_INS_GET_LOCAL);
      emit(a.d);
    }
    void p_jo_emit_set_local() {
      struct dp a = ds_pop();
      char* str = jo2str(a.d);
      emit(JO_INS_SET_LOCAL);
      char* tmp = substring(str, 0, strlen(str) -1);
      emit(str2jo(tmp));
      free(tmp);
    }
    void p_jo_emit_get_field() {
      struct dp a = ds_pop();
      emit(JO_INS_GET_FIELD);
      emit(a.d);
    }
    void p_jo_emit_set_field() {
      struct dp a = ds_pop();
      char* str = jo2str(a.d);
      emit(JO_INS_SET_FIELD);
      char* tmp = substring(str, 0, strlen(str) -1);
      emit(str2jo(tmp));
      free(tmp);
    }
    void p_emit_jz() {
      emit(JO_INS_JZ);
      jo_t* address = tos(compiling_stack);
      p_compiling_stack_inc();
      ds_push(TAG_ADDRESS, address);
    }
    void p_emit_jmp() {
      emit(JO_INS_JMP);
      jo_t* address = tos(compiling_stack);
      p_compiling_stack_inc();
      ds_push(TAG_ADDRESS, address);
    }
    void expose_core() {
      plus_prim("core-flag", p_core_flag);
      plus_prim("core-flag-on", p_core_flag_on);
      plus_prim("core-flag-off", p_core_flag_off);

      plus_prim("bind-name", p_bind_name);

      // note that, the notation of instruction is not exposed to jojo
      plus_prim("jo-emit-call", p_jo_emit_call);
      plus_prim("int-emit-data", p_int_emit_data);
      plus_prim("emit-lit", p_emit_lit);

      plus_prim("jo-emit-get-local", p_jo_emit_get_local);
      plus_prim("jo-emit-set-local", p_jo_emit_set_local);
      plus_prim("jo-emit-get-field", p_jo_emit_get_field);
      plus_prim("jo-emit-set-field", p_jo_emit_set_field);

      plus_prim("emit-jz", p_emit_jz);
      plus_prim("emit-jmp", p_emit_jmp);
    }
    void p1() {
      int file = open("README", O_RDWR);
      struct input_stack* t0_stack = file_input_stack(file);
      input_stack_push(t0_stack, '\n');
      input_stack_push(t0_stack, '\n');
      input_stack_push(t0_stack, '1');
      input_stack_push(t0_stack, '2');
      input_stack_push(t0_stack, '3');
      input_stack_push(t0_stack, '4');
      input_stack_push(t0_stack, '\n');
      input_stack_push(t0_stack, '\n');
      while (!input_stack_empty_p(t0_stack)) {
        report("%c", input_stack_pop(t0_stack));
      }
      input_stack_free(t0_stack);
      report("- input_stack test0 finished\n");

      struct input_stack* t1_stack = terminal_input_stack();
      while (!input_stack_empty_p(t1_stack)) {
        char byte = input_stack_pop(t1_stack);
        report("\n> %c", byte);
      }
      input_stack_free(t1_stack);
      report("- input_stack test1 finished\n");

      struct input_stack* t2_stack = string_input_stack("1234567890");
      input_stack_push(t2_stack, '\n');
      input_stack_push(t2_stack, '\n');
      input_stack_push(t2_stack, '1');
      input_stack_push(t2_stack, '2');
      input_stack_push(t2_stack, '3');
      input_stack_push(t2_stack, '4');
      input_stack_push(t2_stack, '\n');
      input_stack_push(t2_stack, '\n');
      while (!input_stack_empty_p(t2_stack)) {
        char byte = input_stack_pop(t2_stack);
        report("%c", byte);
      }
      input_stack_free(t2_stack);
      report("\n");
      report("- input_stack test2 finished\n");
    }
    void p2() {
      int file = open("k1~",
                      (O_CREAT | O_RDWR),
                      (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));

      struct output_stack* t0_stack = file_output_stack(file);
      output_stack_push(t0_stack, '1'); output_stack_pop(t0_stack);
      output_stack_push(t0_stack, '2');
      output_stack_push(t0_stack, '3'); output_stack_drop(t0_stack);
      output_stack_push(t0_stack, '4');
      output_stack_push(t0_stack, '\n');
      file_output_stack_flush(t0_stack);
      output_stack_push(t0_stack, '1');
      output_stack_push(t0_stack, '2'); output_stack_pop(t0_stack);
      output_stack_push(t0_stack, '3');
      output_stack_push(t0_stack, '4'); output_stack_drop(t0_stack);
      output_stack_push(t0_stack, '\n');
      file_output_stack_flush(t0_stack);
      output_stack_free(t0_stack);
      close(file);
      report("- output_stack test0 finished\n");

      struct output_stack* t1_stack = terminal_output_stack();
      output_stack_push(t1_stack, '\n');
      output_stack_push(t1_stack, '\n');
      output_stack_push(t1_stack, '1');
      output_stack_push(t1_stack, '2');
      output_stack_push(t1_stack, '3');
      output_stack_push(t1_stack, '4');
      output_stack_push(t1_stack, '\n');
      output_stack_push(t1_stack, '\n');
      output_stack_free(t1_stack);
      report("- output_stack test1 finished\n");

      struct output_stack* t2_stack = string_output_stack();
      output_stack_push(t2_stack, '1');
      output_stack_push(t2_stack, '2');
      report("- %c\n", output_stack_pop(t2_stack));
      output_stack_push(t2_stack, '3');
      output_stack_push(t2_stack, '4');
      report("- %c\n", output_stack_pop(t2_stack));
      output_stack_push(t2_stack, '\n');
      char* collected_string = string_output_stack_collect(t2_stack);
      report("- collected_string : %s\n", collected_string);
      output_stack_free(t2_stack);
      report("- output_stack test2 finished\n");
    }
    void p3() {
      struct disp* disp_1 = new_disp(16);
      disp_insert(disp_1, str2jo("k1"), TAG_INT, 100);
      disp_insert(disp_1, str2jo("k1"), TAG_INT, 1);
      disp_insert(disp_1, str2jo("k2"), TAG_INT, 2);
      disp_insert(disp_1, str2jo("k3"), TAG_INT, 3);
      disp_insert(disp_1, str2jo("k4"), TAG_INT, 4);
      disp_insert(disp_1, str2jo("k5"), TAG_INT, 5);
      disp_insert(disp_1, str2jo("k6"), TAG_INT, 6);

      disp_insert(disp_1, str2jo("kkkk1"), TAG_INT, 1);
      disp_insert(disp_1, str2jo("kkkk2"), TAG_INT, 2);
      disp_insert(disp_1, str2jo("kkkk3"), TAG_INT, 3);
      disp_insert(disp_1, str2jo("kkkk4"), TAG_INT, 4);
      disp_insert(disp_1, str2jo("kkkk5"), TAG_INT, 5);
      disp_insert(disp_1, str2jo("kkkk6"), TAG_INT, 6);

      disp_insert(disp_1, str2jo("1"), TAG_INT, 666);
      disp_insert(disp_1, str2jo("2"), TAG_INT, 2);
      disp_insert(disp_1, str2jo("3"), TAG_INT, 3);
      disp_insert(disp_1, str2jo("4"), TAG_INT, 4);
      disp_insert(disp_1, str2jo("5"), TAG_INT, 5);
      disp_insert(disp_1, str2jo("6"), TAG_INT, 6);

      disp_print(disp_1);

      struct disp_entry* disp_entry_1 =
        disp_find(disp_1, str2jo("1"));
      if (disp_entry_1 == 0) {
        report("disp_entry_1 is empty\n");
      }
      else {
        report("disp_entry_1->tag : %s\n", jo2str(disp_entry_1->tag));
        report("disp_entry_1->data : %ld\n", disp_entry_1->data);
      }

      // void* p = 0;
      // printf("- in c stack : %p\n", (void*)&p);
      report("0: %ld\n", 0);
      report("sizeof(struct jotable_entry): %ld\n", sizeof(struct jotable_entry));
    }
    void p4() {
      struct multi_disp* multi_disp_1 = new_multi_disp(16);
      multi_disp_insert(multi_disp_1, J("k1", "k1"), TAG_INT, 100);
      multi_disp_insert(multi_disp_1, J("k1", "k1"), TAG_INT, 1);
      multi_disp_insert(multi_disp_1, J("k21", "k22"), TAG_INT, 2);
      multi_disp_insert(multi_disp_1, J("k31", "k32", "k33"), TAG_INT, 3);
      multi_disp_insert(multi_disp_1, J("k4"), TAG_INT, 4);
      multi_disp_insert(multi_disp_1, J("k5"), TAG_INT, 5);
      multi_disp_insert(multi_disp_1, J("k6"), TAG_INT, 6);

      multi_disp_insert(multi_disp_1, J("kkkk1"), TAG_INT, 1);
      multi_disp_insert(multi_disp_1, J("kkkk2"), TAG_INT, 2);
      multi_disp_insert(multi_disp_1, J("kkkk3"), TAG_INT, 3);
      multi_disp_insert(multi_disp_1, J("kkkk4"), TAG_INT, 4);
      multi_disp_insert(multi_disp_1, J("kkkk5"), TAG_INT, 5);
      multi_disp_insert(multi_disp_1, J("kkkk6"), TAG_INT, 6);

      multi_disp_insert(multi_disp_1, J("1", "2", "3"), TAG_INT, 666);
      multi_disp_insert(multi_disp_1, J("2"), TAG_INT, 2);
      multi_disp_insert(multi_disp_1, J("3"), TAG_INT, 3);
      multi_disp_insert(multi_disp_1, J("4"), TAG_INT, 4);
      multi_disp_insert(multi_disp_1, J("5"), TAG_INT, 5);
      multi_disp_insert(multi_disp_1, J("6"), TAG_INT, 6);

      multi_disp_print(multi_disp_1);

      struct multi_disp_entry* multi_disp_entry_1 =
        multi_disp_find(multi_disp_1, J("1", "2", "3"));
      if (multi_disp_entry_1 == 0) {
        report("multi_disp_entry_1 is empty\n");
      }
      else {
        report("multi_disp_entry_1->tag : %s\n", jo2str(multi_disp_entry_1->tag));
        report("multi_disp_entry_1->data : %ld\n", multi_disp_entry_1->data);
      }

      struct multi_disp_entry* multi_disp_entry_2 =
        multi_disp_find(multi_disp_1, J("1", "2", "3123"));
      if (multi_disp_entry_2 == 0) {
        report("multi_disp_entry_2 is empty\n");
      }
      else {
        report("multi_disp_entry_2->tag : %s\n", jo2str(multi_disp_entry_2->tag));
        report("multi_disp_entry_2->data : %ld\n", multi_disp_entry_2->data);
      }
    }
    void p5() {
      report("sizeof(cell) : %ld\n", sizeof(cell));
      report("sizeof(jo_t) : %ld\n", sizeof(jo_t));
    }
    void path_load(char* path) {
      int file = open(path, O_RDONLY);
      if(file == -1) {
        report("- path_load fail : %s\n", path);
        perror("file open failed");
        return;
      }
      repl(file_input_stack(file));
      close(file);
    }
    void expose_play() {
      plus_prim("p1", p1);
      plus_prim("p2", p2);
      plus_prim("p3", p3);
      plus_prim("p4", p4);
      plus_prim("p5", p5);
      plus_prim("print-data-stack", p_print_ds);
    }
    void init_system() {
      setvbuf(stdout, 0, _IONBF, 0);
      setvbuf(stderr, 0, _IONBF, 0);
    }
    void init_jotable() {
      bzero(jotable, JOTABLE_SIZE * sizeof(struct jotable_entry));
    }
    void init_literal_jo() {
      EMPTY_JO = str2jo("");

      TAG_PRIM         = str2jo("<prim>");
      TAG_JOJO         = str2jo("<jojo>");
      TAG_CLOSURE      = str2jo("<closure>");
      TAG_ADDRESS      = str2jo("<address>");
      TAG_CLASS        = str2jo("<class>");

      TAG_BOOL         = str2jo("<bool>");
      TAG_INT          = str2jo("<int>");
      TAG_BYTE         = str2jo("<byte>");
      TAG_STRING       = str2jo("<string>");
      TAG_JO           = str2jo("<jo>");

      TAG_UNINITIALISED_FIELD_PLACE_HOLDER =
        str2jo("<uninitialised-field-place-holder>");

      ROUND_BAR    =   str2jo("(");
      ROUND_KET    =   str2jo(")");
      SQUARE_BAR   =   str2jo("[");
      SQUARE_KET   =   str2jo("]");
      FLOWER_BAR   =   str2jo("{");
      FLOWER_KET   =   str2jo("}");
      DOUBLEQUOTE  =   str2jo("\"");
      SINGLEQUOTE  =   str2jo("'");
      QUASIQUOTE   =   str2jo("`");
      COMMA        =   str2jo(",");

      JO_INS_LIT       = str2jo("ins/lit");
      JO_INS_GET_LOCAL = str2jo("ins/get-local");
      JO_INS_SET_LOCAL = str2jo("ins/set-local");
      JO_INS_GET_FIELD = str2jo("ins/get-field");
      JO_INS_SET_FIELD = str2jo("ins/set-field");

      JO_INS_JMP = str2jo("ins/jmp");
      JO_INS_JZ  = str2jo("ins/jz");

      JO_INS_TAIL_CALL = str2jo("ins/tail-call");
      JO_INS_LOOP = str2jo("ins/loop");
      JO_INS_RECUR = str2jo("ins/recur");

      JO_NULL     = str2jo("null");
      JO_THEN     = str2jo("then");
      JO_ELSE     = str2jo("else");

      JO_APPLY     = str2jo("apply");
      JO_EXE       = str2jo("exe");
      JO_END       = str2jo("end");
      JO_RECUR     = str2jo("recur");
      JO_CLOSURE   = str2jo("closure");

      JO_CURRENT_LOCAL_ENV   = str2jo("current-local-env");

      JO_LOCAL_DATA_IN = str2jo("local-data-in");
      JO_LOCAL_DATA_OUT = str2jo("local-data-out");

      JO_LOCAL_TAG_IN = str2jo("local-tag-in");
      JO_LOCAL_TAG_OUT = str2jo("local-tag-out");

      JO_LOCAL_IN = str2jo("local-in");
      JO_LOCAL_OUT = str2jo("local-out");
    }
    jo_t jojo_area[1024 * 1024];

    void init_stacks() {
      ds = new_stack("ds");
      rs = new_stack("rs");

      compiling_stack = new_stack("compiling_stack");
      push(compiling_stack, jojo_area);

      reading_stack = new_stack("reading_stack");

      writing_stack = new_stack("writing_stack");
      push(writing_stack, terminal_output_stack());
    }
    void init_expose() {
      expose_gc();
      expose_gene();
      expose_stack();
      expose_ending();
      expose_rw();
      expose_io();
      expose_local();
      expose_compiler();
      expose_control();
      expose_top();
      expose_repl();
      expose_step();
      expose_bool();
      expose_string();
      expose_int();
      expose_jo();
      expose_address();
      expose_jojo();
      expose_closure();
      expose_socket();
      expose_system();
      expose_core();
      expose_play();
    }
    int main(int argc, char** argv) {
      cmd_number = argc;
      cmd_string_array = argv;

      init_system();
      init_jotable();
      init_literal_jo();
      init_stacks();
      init_expose();
      init_kernel_signal_handler();

      // load_core();
      path_load("core.jo");

      p_repl_flag_on();
      repl(terminal_input_stack());
    }
