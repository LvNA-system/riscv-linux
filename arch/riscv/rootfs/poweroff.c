#include <sys/reboot.h>
#include <unistd.h>

int main() {
  reboot(RB_POWER_OFF);
  for (;;)
    sleep(1);
  return 0;
}
