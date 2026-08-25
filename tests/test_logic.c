#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Extrait de la logique de la phase 19 (TaskMonitor), sans le GPIO
static bool led_should_be_on(uint32_t value, uint32_t threshold)
{
  return value > threshold;
}

// Extrait de la logique de la phase 23 (validation avant ecriture Flash)
static bool firmware_size_is_valid(uint32_t size, uint32_t max_size)
{
  if (size == 0U) { return false; }
  if (size > max_size) { return false; }
  if ((size % 8U) != 0U) { return false; }
  return true;
}

static void test_led_threshold(void)
{
  assert(led_should_be_on(2049, 2048) == true);
  assert(led_should_be_on(2048, 2048) == false);   // egal au seuil -> pas au-dessus
  assert(led_should_be_on(0, 2048) == false);
  assert(led_should_be_on(4095, 2048) == true);
  printf("test_led_threshold : OK\n");
}

static void test_firmware_size(void)
{
  assert(firmware_size_is_valid(0, 4096) == false);      // taille nulle rejetee
  assert(firmware_size_is_valid(4104, 4096) == false);   // trop grand rejete
  assert(firmware_size_is_valid(10, 4096) == false);     // pas multiple de 8
  assert(firmware_size_is_valid(8, 4096) == true);
  assert(firmware_size_is_valid(4096, 4096) == true);    // limite exacte acceptee
  printf("test_firmware_size : OK\n");
}

int main(void)
{
  test_led_threshold();
  test_firmware_size();
  printf("Tous les tests sont passes.\n");
  return 0;
}