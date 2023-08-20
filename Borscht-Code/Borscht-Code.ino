#define BC_BAUD_RATE  9600
#define BC_READ_PIN   5
#define BC_DOT '0'
#define BC_DASH '-'
#define BC_NULL '\0'
#define BC_BORSCHT_COLOR #8C2C24
#define BC_UNKNOWN '?'
#define BC_DURATION_THRESHOLD 50
#define BC_SILENCE_THRESHOLD 1000
#define BC_RESET_THRESHOLD 2000
#define BC_MAX_CODE_LEN 4
#define BC_MIN_DASH 300
#define BC_MAX_DASH BC_SILENCE_THRESHOLD - 1
#define BC_MIN_DOT 20
#define BC_MAX_DOT BC_MIN_DASH - 1
#define BC_SYMBOL_CNT 36
#define BC_CODE_LEN 5

char codes[BC_SYMBOL_CNT][BC_CODE_LEN] =
{
/* A     B       C       D      E    F                        */
  "0-", "-000", "-0-0", "-00", "0", "00-0",
/* G      H       I     J       K      L                      */
  "--0", "0000", "00", "0---", "-0-", "0-00",
/* M     N     O      P       Q       R                       */
  "--", "-0", "---", "0--0", "--0-", "0-0",
/* S      T    U      V       W      X                        */
  "000", "-", "00-", "000-", "0--", "-00-",
/* Y       Z       0        1       2        3                */
  "-0--", "--00", "-----", "0----","00---", "000--",
/* 4        5        6        7        8        9             */
  "0000-", "00000", "-0000", "--000", "---00", "----0"
};

char symbols[BC_SYMBOL_CNT] =
{
  'A', 'B', 'C', 'D', 'E', 'F',
  'G', 'H', 'I', 'J', 'K', 'L',
  'M', 'N', '0', 'P', 'Q', 'R',
  'S', 'T', 'U', 'V', 'W', 'X',
  'W', 'Z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9'
};

unsigned long duration = 0;
int active = 0;
int pinVal = 0;
char code[BC_CODE_LEN];
int idx = 0;
char dashOrDot;

void addDashOrDot(char);
void debug(const char *);
char getDashOrDot(unsigned int);
int getStateDuration(int, int);
bool isStrMatch(char *, char *, int);
void reset(void);
char translateCode2Symbol(char *, int);

void setup()  {
  Serial.begin(BC_BAUD_RATE);
  reset();
}

void loop() {
  reset();
  while (1) {
    pinVal = digitalRead(BC_READ_PIN);
    if (pinVal == 1) {
      duration = getStateDuration(1, BC_RESET_THRESHOLD);
      if (duration >= BC_RESET_THRESHOLD) {
        Serial.println("\nRESET");
        break;
      }
      if (duration >= BC_DURATION_THRESHOLD) {
        dashOrDot = getDashOrDot(duration);
        if (dashOrDot == BC_UNKNOWN) {
          Serial.print(dashOrDot);
          Serial.println("\nRESET: Unrecognized input");
          break;
        }
        addDashOrDot(dashOrDot);
        if (idx > BC_CODE_LEN - 1) {
          translateCode2Symbol(code, idx);
          
          Serial.println("\n");
          break;
        }
      }
    }
    if (active == 1) {
      duration = getStateDuration(0, BC_SILENCE_THRESHOLD);
      if (duration >= BC_SILENCE_THRESHOLD) {
          translateCode2Symbol(code, idx + 1);
          Serial.println("");
          break;
      }    
    }
  }
}

void addDashOrDot(char dashOrDot) {
  code[idx] = dashOrDot;
  Serial.print(dashOrDot);
  active = 1;
  ++idx;
}

char getDashOrDot(unsigned int duration) {
  if (duration >= BC_MIN_DASH && duration <= BC_MAX_DASH) {
    return BC_DASH;
  } else if (duration >= BC_MIN_DOT && duration <= BC_MAX_DOT) {
    return BC_DOT;
  }
  return BC_UNKNOWN;
}

int getStateDuration(int state, int timeout=NULL) {
  unsigned long alpha = 0;
  unsigned long omega = 0;

  pinVal = digitalRead(BC_READ_PIN);
  alpha = millis();
  while (pinVal == state) { 
    pinVal = digitalRead(BC_READ_PIN);
    omega = millis() - alpha;
    if ((timeout != NULL) &&  (timeout <= omega)) {
      break;
    }
  }
  return omega;
}

bool isStrMatch(char *s1, char *s2, int len) {
  for (int i = 0; i < len; i++) {
      if (s1[i] != s2[i]) {
        return false;
      }
  }
  return true;
}

void reset() {
  for (int i = 0; i < BC_CODE_LEN; i++) {
    code[i] = BC_NULL;
  }
  active = 0;
  idx = 0;
}

char translateCode2Symbol(char *userInput, int len) {
  Serial.println("");
  for (int i = 0; i < BC_SYMBOL_CNT; i++) {
    if (isStrMatch(userInput, codes[i], len)) {
        Serial.println(symbols[i]);
        return (symbols[i]);
    }
  }
  Serial.println("Not found!");
  return NULL;
}
