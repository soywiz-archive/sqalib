<?php

require_once(__DIR__ . '/builder.php');

//$common = '/MD /GF /Ox /EHsc /DSQUSEDOUBLE=1';
$common = '/MD /GF /Ox /EHsc';

// Squirrel.
Builder::build(array(
	'out' => 'squirrel/squirrel.lib',
	'in'  => array(
		'squirrel/*.cpp',
	),
	'include' => array(
		'squirrel',
	),
	'opts' => "{$common}",
));

// Sqstdlib.lib
Builder::build(array(
	'out' => 'sqstdlib/sqstdlib.lib',
	'in'  => array(
		'sqstdlib/*.cpp',
	),
	'include' => array(
		'squirrel',
		'sqstdlib',
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
		'squirrel',
		'sqstdlib',
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
		'squirrel/squirrel.lib',
		'sqstdlib/sqstdlib.lib',
		'sqalib/sqalib.lib',
	),
	'include' => array(
		'squirrel',
		'sqstdlib',
		'sqalib',
		'sqrat',
	),
	'opts' => "{$common}",
));
