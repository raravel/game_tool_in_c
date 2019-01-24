Game Tool in C
========
개요
------
Game Tool 은 Windows에서 C언어로 게임을 만들고자 하는 사람들을 위한 라이브러리입니다. 구현하려면 복잡한 기능들을 함수 하나로 모아두었습니다.

기본적으로 [D2Coding](https://github.com/naver/d2codingfont "D2Coding") 폰트를 사용합니다.
그러나 D2Coding 폰트가 없어도 직접 지정할 수 있습니다.

사용법
------
example 폴더에 예제가 있습니다.

### initializer / finalize
------
main 함수가 시작되기 전, 끝난 후 실행되는 함수가 있습니다.
game_tool.c 에서 확인하고, 변경할 수 있습니다.

### common
------

#### 콘솔 크기
열려있는 콘솔창에서 사용 가능한 width와 height의 크기를 구합니다.
미리 정의된 변수로 사용하거나 직접 함수를 호출할 수 있습니다.
```C
#define CONSOLE_WIDTH get_console_width();
#define CONSOLE_HEIGHT get_console_height();
```

### 콘솔 폰트
기본적으로 D2Coding을 사용하는 콘솔의 폰트를 변경할 수 있습니다.
game_tool.h 에서 직접 변경하세요. 
```c
#define CONSOLE_FONT L"D2Coding"
```

### log
------
각 레벨에 로그를 출력하고, 원하는 레벨까지의 로그를 출력되도록 세팅할 수 있습니다.
```c
SET_LOG_LVL(LOG_NONE);
```
레벨에 대한 enum은 다음과 같습니다.
```c
typedef enum _log_level {
	LOG_NONE = 0,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_TEST
} log_level;
```
로그를 추가하려면 다음 함수를 사용합니다.
```c
LOGGING(LOG_TEST, "print log file %s", __FILE__);
```
### console
------
여러가지 콘솔에 관한 처리를 다룹니다.

#### 현재 커서 위치
현재 출력되고있는 커서의 위치좌표를 구할 수 있습니다.
```c
COORD pos = get_now_cursor_position();
```

#### 콘솔 커서 이동
출력되는 위치의 커서를 이동시킬 수 있습니다.
```c
gotoxy(x, y);
```

#### 커서 보이기/숨기기
커서가 보일지 숨길지 설정할 수 있습니다.
기본적으로 보이지 않게 되어있습니다.
```c
cursor_view(hide);
cursor_view(visible);
```

#### 콘솔 색 변경
출력되는 내용의 색을 변경시킬 수 있습니다.
기본값은 배경 : 검정, 전경 : 하양 입니다.
색과 관련된 enum은 다음과 같습니다.
```c
typedef enum _color {
	CR_BLACK = 0,
	CR_DARK_BLUE,
	CR_DARK_GREEN,
	CR_DARK_SKYBLUE,
	CR_DARK_RED,
	CR_DARK_PINK,
	CR_DARK_YELLOW,
	CR_SILVER,
	CR_GRAY,
	CR_BLUE,
	CR_GREEN,
	CR_SKYBLUE,
	CR_RED,
	CR_PINK,
	CR_YELLOW,
	CR_WHITE
} color;
```
콘솔에 출력되는 색은 SetColor 와 SetColorS함수를 사용하여 변경할 수 있습니다.
```c
COLORS cr = {CR_BLACK, CR_PINK};
SetColor(CR_BLACK, CR_PINK);
SetColorS(cr);
```
현재 콘솔의 색과, 바뀌기 이전 색은 다음에 정의되어 있습니다.
```
__NOW_CONSOLE_COLOR__ //현재 색
__BEFORE_CONSOLE_COLOR__; //현재 이전의 색
```

### progress bar
------

#### 선언
progressbar 는 구조체입니다. 다음과 같이 선언할 수 있습니다.
```c
progressbar pgbar = new_progreebar();
```

#### 초기화
필수적으로 progressbar 의 크기와 위치를 초기화 해주어야 합니다.
```c
pgbar.size = (COORD) { 10, 1 };
pgbar.position = (COORD) { 0, 0 };
```

#### 진행
progressbar는 한칸씩 진행시킬 수 있습니다.
```c
progressbar_step(&pgbar);
```

#### 스타일
progressbar 의 1칸당 문자를 지정할 수 있습니다.
기본은 "─" 입니다.
```c
pgbar.progress = "■";
```

progressbar의 배경색과 전경색을 지정할 수 있습니다.
기본적으로 배경 : CR_BLACK 전경 : CR_GREEN 입니다.
```c
pgbar.color (COLORS) { CR_GREEN, CR_WHITE };
```

#### 딜레이
progressbar가 진행될 때 기다리는 시간을 지정할 수 있습니다.
단위는 millisecond 입니다. 기본값은 100입니다.
```c
pgbar.animation_delay = 10;
```

#### 최대값
progressbar는 최대값과 progressbar의 크기를 계산하여 진행도를 나타냅니다.
최대값은 기본적으로 100입니다.
```c
pgbar.max_value = 50;
```

#### 실행/종료 여부
progressbar 가 현재 동작중인지, 100%를 채웠는지 확인하는 플래그입니다.
```c
pgbar.is_running (bool)
pgbar.is_complete (bool)
```

#### 타입
progressbar 는 두가지의 타입이 있습니다.
1. 현재 진행도를 표시하는 block 타입
2. 작업 진행중을 알리는 infinity 타입

기본값은 block 타입입니다.
```c
pgbar.type = block;
pgbar.type = infinity;
```

#### 쓰레드
progressbar 의 타입이 infinity 일 때, 프로그래스바가 진행되는 쓰레드를 생성하여 별 다른 처리없이 진행시킬 수 있습니다.
기본값은 false 입니다.

단, 프로그램의 전체 스레드에서 progressbar는 하나의 스레드에서만 다룰 수 있습니다.
여러 progressbar를 사용해야 하는 경우라면 false로 직접 처리하세요.
```c
pgbar.use_thread = true;
progress_step(&pgbar);
```
