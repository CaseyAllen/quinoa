; ModuleID = 'Quinoa Program'
source_filename = "Quinoa Program"

@prop._eyOp3dKB47_.IO.stdout = linkonce global i32 1

define i32 @fn.Main.main.PR_int32.PR_int8-p-p(i32 %argc, i8** %argv) {
entry_block:
  %"param argc" = alloca i32, align 4
  store i32 %argc, i32* %"param argc", align 4
  %"param argv" = alloca i8**, align 8
  store i8** %argv, i8*** %"param argv", align 8
  ret i32 0
}

define i64 @fn._eyOp3dKB47_.IO.println.PR_int8-p(i8* %message) {
entry_block:
  %"param message" = alloca i8*, align 8
  store i8* %message, i8** %"param message", align 8
  %len = alloca i64, align 8
  %0 = load i8*, i8** %"param message", align 8
  %1 = call i64 @fn._eyOp3dKB47_.IO.print.PR_int8-p(i8* %0)
  store i64 %1, i64* %len, align 4
  call void @fn._eyOp3dKB47_.IO.putc.PR_int8(i8 10)
  %2 = load i64, i64* %len, align 4
  %3 = add i64 %2, 1
  ret i64 %3
}

define i64 @fn._eyOp3dKB47_.IO.print.PR_int8-p(i8* %message) {
entry_block:
  %"param message" = alloca i8*, align 8
  store i8* %message, i8** %"param message", align 8
  %len = alloca i64, align 8
  %0 = load i32, i32* @prop._eyOp3dKB47_.IO.stdout, align 4
  %1 = load i8*, i8** %"param message", align 8
  %2 = ptrtoint i8* %1 to i64
  %3 = load i8*, i8** %"param message", align 8
  %4 = call i64 @fn._IloKron5Wi_.CString.len.PR_int8-p(i8* %3)
  %5 = call i64 @write(i32 %0, i64 %2, i64 %4)
  store i64 %5, i64* %len, align 4
  %6 = load i64, i64* %len, align 4
  ret i64 %6
}

define void @fn._eyOp3dKB47_.IO.putc.PR_int8(i8 %ch) {
entry_block:
  %"param ch" = alloca i8, align 1
  store i8 %ch, i8* %"param ch", align 1
  %0 = load i32, i32* @prop._eyOp3dKB47_.IO.stdout, align 4
  %1 = ptrtoint i8* %"param ch" to i64
  %2 = call i64 @write(i32 %0, i64 %1, i64 1)
  ret void
}

declare i64 @write(i32, i64, i64)

define i64 @fn._eyOp3dKB47_.IO.read.PR_int8-p.PR_int64(i8* %str, i64 %len) {
entry_block:
  %"param str" = alloca i8*, align 8
  store i8* %str, i8** %"param str", align 8
  %"param len" = alloca i64, align 8
  store i64 %len, i64* %"param len", align 4
  %0 = load i8*, i8** %"param str", align 8
  %1 = ptrtoint i8* %0 to i64
  %2 = load i64, i64* %"param len", align 4
  %3 = call i64 @read(i64 0, i64 %1, i64 %2)
  ret i64 %3
}

declare i64 @read(i64, i64, i64)

define i64 @fn._eyOp3dKB47_.IO.strlen.PR_int8-p(i8* %str) {
entry_block:
  %"param str" = alloca i8*, align 8
  store i8* %str, i8** %"param str", align 8
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  br label %while_eval

while_eval:                                       ; preds = %while_exec, %entry_block
  %0 = load i64, i64* %i, align 4
  %1 = load i8*, i8** %"param str", align 8
  %2 = getelementptr i8, i8* %1, i64 %0
  %3 = load i8, i8* %2, align 1
  %4 = icmp ne i8 %3, 0
  br i1 %4, label %while_exec, label %while_cont

while_exec:                                       ; preds = %while_eval
  %5 = load i64, i64* %i, align 4
  %6 = add i64 %5, 1
  store i64 %6, i64* %i, align 4
  br label %while_eval

while_cont:                                       ; preds = %while_eval
  %7 = load i64, i64* %i, align 4
  ret i64 %7
}

define i64 @fn._IloKron5Wi_.CString.len.PR_int8-p(i8* %str) {
entry_block:
  %"param str" = alloca i8*, align 8
  store i8* %str, i8** %"param str", align 8
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  br label %while_eval

while_eval:                                       ; preds = %while_exec, %entry_block
  %0 = load i64, i64* %i, align 4
  %1 = load i8*, i8** %"param str", align 8
  %2 = getelementptr i8, i8* %1, i64 %0
  %3 = load i8, i8* %2, align 1
  %4 = icmp ne i8 %3, 0
  br i1 %4, label %while_exec, label %while_cont

while_exec:                                       ; preds = %while_eval
  %5 = load i64, i64* %i, align 4
  %6 = add i64 %5, 1
  store i64 %6, i64* %i, align 4
  br label %while_eval

while_cont:                                       ; preds = %while_eval
  %7 = load i64, i64* %i, align 4
  ret i64 %7
}

define i32 @main(i32 %argc, i8** %argv) {
main_entry:
  %0 = call i32 @fn.Main.main.PR_int32.PR_int8-p-p(i32 %argc, i8** %argv)
  ret i32 %0
}
