@.str = private constant [5 x i8] c"%d \0A\00", align 1 ; <[5 x i8]*> [#uses=1]
@init = global i32 0                              ; <i32*> [#uses=1]
@WIDTH = constant %struct.fifo_i16_s* null ; <%struct.fifo_i16_s**> [#uses=3]
@HEIGHT = constant %struct.fifo_i16_s* null ; <%struct.fifo_i16_s**> [#uses=3]
@B = constant %struct.fifo_i8_s* null ; <%struct.fifo_i8_s**> [#uses=3]

define i32 @scheduler() nounwind {
entry:
  %i = alloca i32                                 ; <i32*> [#uses=5]
  %ptr = alloca i16*                              ; <i16**> [#uses=4]
  %width = alloca i16                             ; <i16*> [#uses=2]
  %height = alloca i16                            ; <i16*> [#uses=2]
  %ptr5 = alloca i8*                              ; <i8**> [#uses=2]
  %"alloca point" = bitcast i32 0 to i32          ; <i32> [#uses=0]
  store i32 0, i32* %i, align 4
  br label %bb

bb:                                               ; preds = %bb7, %entry
  %0 = load %struct.fifo_i16_s** @WIDTH, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  %1 = call i32 @fifo_i16_has_tokens(%struct.fifo_i16_s* %0, i32 1) nounwind ; <i32> [#uses=1]
  %2 = icmp ne i32 %1, 0                          ; <i1> [#uses=1]
  br i1 %2, label %bb1, label %bb3

bb1:                                              ; preds = %bb
  %3 = load %struct.fifo_i16_s** @HEIGHT, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  %4 = call i32 @fifo_i16_has_tokens(%struct.fifo_i16_s* %3, i32 1) nounwind ; <i32> [#uses=1]
  %5 = icmp ne i32 %4, 0                          ; <i1> [#uses=1]
  br i1 %5, label %bb2, label %bb3

bb2:                                              ; preds = %bb1
  %6 = load %struct.fifo_i16_s** @WIDTH, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  %7 = call i16* @fifo_i16_read(%struct.fifo_i16_s* %6, i32 1) nounwind ; <i16*> [#uses=1]
  store i16* %7, i16** %ptr, align 4
  %8 = load i16** %ptr, align 4                   ; <i16*> [#uses=1]
  %9 = getelementptr inbounds i16* %8, i32 0      ; <i16*> [#uses=1]
  %10 = load i16* %9, align 1                     ; <i16> [#uses=1]
  %11 = sext i16 %10 to i32                       ; <i32> [#uses=1]
  %12 = mul nsw i32 %11, 16                       ; <i32> [#uses=1]
  %13 = trunc i32 %12 to i16                      ; <i16> [#uses=1]
  store i16 %13, i16* %width, align 2
  %14 = load %struct.fifo_i16_s** @WIDTH, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  call void @fifo_i16_read_end(%struct.fifo_i16_s* %14, i32 1) nounwind
  %15 = load %struct.fifo_i16_s** @HEIGHT, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  %16 = call i16* @fifo_i16_read(%struct.fifo_i16_s* %15, i32 1) nounwind ; <i16*> [#uses=1]
  store i16* %16, i16** %ptr, align 4
  %17 = load i16** %ptr, align 4                  ; <i16*> [#uses=1]
  %18 = getelementptr inbounds i16* %17, i32 0    ; <i16*> [#uses=1]
  %19 = load i16* %18, align 1                    ; <i16> [#uses=1]
  %20 = sext i16 %19 to i32                       ; <i32> [#uses=1]
  %21 = mul nsw i32 %20, 16                       ; <i32> [#uses=1]
  %22 = trunc i32 %21 to i16                      ; <i16> [#uses=1]
  store i16 %22, i16* %height, align 2
  %23 = load %struct.fifo_i16_s** @HEIGHT, align 4 ; <%struct.fifo_i16_s*> [#uses=1]
  call void @fifo_i16_read_end(%struct.fifo_i16_s* %23, i32 1) nounwind
  %24 = load i16* %height, align 2                ; <i16> [#uses=1]
  %25 = sext i16 %24 to i32                       ; <i32> [#uses=1]
  %26 = load i16* %width, align 2                 ; <i16> [#uses=1]
  %27 = sext i16 %26 to i32                       ; <i32> [#uses=1]
  call void @set_video(i32 %27, i32 %25) nounwind
  %28 = load i32* %i, align 4                     ; <i32> [#uses=1]
  %29 = add nsw i32 %28, 1                        ; <i32> [#uses=1]
  store i32 %29, i32* %i, align 4
  br label %bb3

bb3:                                              ; preds = %bb2, %bb1, %bb
  %30 = load %struct.fifo_i8_s** @B, align 4 ; <%struct.fifo_i8_s*> [#uses=1]
  %31 = call i32 @fifo_i8_has_tokens(%struct.fifo_i8_s* %30, i32 384) nounwind ; <i32> [#uses=1]
  %32 = icmp ne i32 %31, 0                        ; <i1> [#uses=1]
  br i1 %32, label %bb4, label %bb8

bb4:                                              ; preds = %bb3
  %33 = load i32* @init, align 4                  ; <i32> [#uses=1]
  %34 = icmp eq i32 %33, 0                        ; <i1> [#uses=1]
  br i1 %34, label %bb6, label %bb7

bb6:                                              ; preds = %bb4
  call void (...)* @set_init() nounwind
  br label %bb7

bb7:                                              ; preds = %bb6, %bb4
  %35 = load %struct.fifo_i8_s** @B, align 4 ; <%struct.fifo_i8_s*> [#uses=1]
  %36 = call i8* @fifo_i8_read(%struct.fifo_i8_s* %35, i32 384) nounwind ; <i8*> [#uses=1]
  store i8* %36, i8** %ptr5, align 4
  %37 = load i8** %ptr5, align 4                  ; <i8*> [#uses=1]
  call void @write_mb(i8* %37) nounwind
  %38 = load %struct.fifo_i8_s** @B, align 4 ; <%struct.fifo_i8_s*> [#uses=1]
  call void @fifo_i8_read_end(%struct.fifo_i8_s* %38, i32 384) nounwind
  %39 = load i32* %i, align 4                     ; <i32> [#uses=1]
  %40 = add nsw i32 %39, 1                        ; <i32> [#uses=1]
  store i32 %40, i32* %i, align 4
  br label %bb

bb8:                                              ; preds = %bb3
  br label %return

return:                                           ; preds = %bb8
  ret i32 1
}

!source = !{!0}
!name = !{!1}
!action_scheduler = !{!2}
!inputs = !{!3, !5, !7}
!state_variables = !{!9}
!procedures = !{!12, !13, !14}

!0 = metadata !{metadata !"tools/Display.bc"}
!1 = metadata !{metadata !"Display"}
!2 = metadata !{null, i32()* @scheduler}
!3 = metadata !{metadata !4, metadata !"B", %struct.fifo_i8_s** @B}
!4 = metadata  !{ i32 8 ,  null }
!5 = metadata !{metadata !6, metadata !"WIDTH", %struct.fifo_i16_s** @WIDTH}
!6 = metadata  !{ i32 16 ,  null }
!7 = metadata !{metadata !8, metadata !"HEIGHT", %struct.fifo_i16_s** @HEIGHT}
!8 = metadata  !{ i32 16 ,  null }
!9 = metadata !{metadata !10, metadata !11, i32* @init}
!10 = metadata !{metadata !"init", i1 0, i32 0,  i32 0}
!11 = metadata  !{ i32 32 ,  null }
!12 = metadata !{metadata !"set_video", i1 1 , void(i32, i32)* @set_video}
!13 = metadata !{metadata !"set_init", i1 1 , void(...)* @set_init}
!14 = metadata !{metadata !"write_mb", i1 1 , void(i8*)* @write_mb}

declare void @set_video(i32, i32)

declare void @set_init()

declare void @write_mb(i8*)

%struct.FILE = type { i8*, i32, i8*, i32, i32, i32, i32, i8* }
%struct.fifo_i8_s = type opaque
%struct.fifo_i32_s = type opaque
%struct.fifo_i16_s = type opaque

declare i32 @fifo_i8_has_tokens(%struct.fifo_i8_s* %fifo, i32 %n)
declare i32 @fifo_i8_has_room(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_i8_peek(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_i8_read(%struct.fifo_i8_s* %fifo, i32 %n)
declare void @fifo_i8_read_end(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_i8_write(%struct.fifo_i8_s* %fifo, i32 %n)
declare void @fifo_i8_write_end(%struct.fifo_i8_s* %fifo, i32 %n)
declare i32 @fifo_u_i8_has_tokens(%struct.fifo_i8_s* %fifo, i32 %n)
declare i32 @fifo_u_i8_has_room(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_u_i8_peek(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_u_i8_read(%struct.fifo_i8_s* %fifo, i32 %n)
declare void @fifo_u_i8_read_end(%struct.fifo_i8_s* %fifo, i32 %n)
declare i8* @fifo_u_i8_write(%struct.fifo_i8_s* %fifo, i32 %n)
declare void @fifo_u_i8_write_end(%struct.fifo_i8_s* %fifo, i32 %n)
declare i32 @fifo_i16_has_tokens(%struct.fifo_i16_s* %fifo, i32 %n)
declare i32 @fifo_i16_has_room(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_i16_peek(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_i16_read(%struct.fifo_i16_s* %fifo, i32 %n)
declare void @fifo_i16_read_end(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_i16_write(%struct.fifo_i16_s* %fifo, i32 %n)
declare void @fifo_i16_write_end(%struct.fifo_i16_s* %fifo, i32 %n)
declare i32 @fifo_u_i16_has_tokens(%struct.fifo_i16_s* %fifo, i32 %n)
declare i32 @fifo_u_i16_has_room(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_u_i16_peek(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_u_i16_read(%struct.fifo_i16_s* %fifo, i32 %n) 
declare void @fifo_u_i16_read_end(%struct.fifo_i16_s* %fifo, i32 %n)
declare i16* @fifo_u_i16_write(%struct.fifo_i16_s* %fifo, i32 %n)
declare void @fifo_u_i16_write_end(%struct.fifo_i16_s* %fifo, i32 %n)
declare i32 @fifo_i32_has_tokens(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32 @fifo_i32_has_room(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_i32_peek(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_i32_read(%struct.fifo_i32_s* %fifo, i32 %n)
declare void @fifo_i32_read_end(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_i32_write(%struct.fifo_i32_s* %fifo, i32 %n)
declare void @fifo_i32_write_end(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32 @fifo_u_i32_has_tokens(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32 @fifo_u_i32_has_room(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_u_i32_peek(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_u_i32_read(%struct.fifo_i32_s* %fifo, i32 %n)
declare void @fifo_u_i32_read_end(%struct.fifo_i32_s* %fifo, i32 %n)
declare i32* @fifo_u_i32_write(%struct.fifo_i32_s* %fifo, i32 %n)
declare void @fifo_u_i32_write_end(%struct.fifo_i32_s* %fifo, i32 %n) 