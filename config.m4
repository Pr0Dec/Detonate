PHP_ARG_ENABLE(detonate, whether to enable detonate support,
[ --enable-detonate   Enable PHP eval hook support - by SU - @SuperUserek])
if test "$PHP_DETONATE" = "yes"; then
  AC_DEFINE(HAVE_DETONATE, 1, [Whether you have PHP detonate])
  PHP_NEW_EXTENSION(detonate, detonate.c, $ext_shared)
fi