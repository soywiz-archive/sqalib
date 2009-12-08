<?php

require_once(__DIR__ . '/builder.php');

//$common = '/MD /GF /Ox /EHsc /DSQUSEDOUBLE=1';
$common = '/MD /GF /Ox /EHsc';

// Squirrel.
Builder::build(array(
	'out' => 'squirrel3/squirrel.lib',
	'in'  => array(
		'squirrel3/*.cpp',
	),
	'include' => array(
		'squirrel3',
	),
	'opts' => "{$common}",
));

// Sqstdlib.lib
Builder::build(array(
	'out' => 'squirrel3/sqstdlib/sqstdlib.lib',
	'in'  => array(
		'squirrel3/sqstdlib/*.cpp',
	),
	'include' => array(
		'squirrel3',
		'squirrel3/sqstdlib',
	),
	'opts' => "{$common}",
));

// Sqalib.lib
Builder::build(array(
	'out' => 'sqalib/sqalib.lib',
	'in'  => array(
		'sqalib/*.cpp',
	),
	'include' => array(
		'squirrel3',
		'squirrel3/sqstdlib',
		'sqalib',
		'sqrat',
	),
	'opts' => "{$common}",
));

// SQ.exe
Builder::build(array(
	'out' => 'sq.exe',
	'in'  => array(
		'sq.c',
		'squirrel3/squirrel.lib',
		'squirrel3/sqstdlib/sqstdlib.lib',
		'sqalib/sqalib.lib',
	),
	'include' => array(
		'squirrel3',
		'squirrel3/sqstdlib',
		'sqalib',
		'sqrat',
	),
	'opts' => "{$common}",
));
