<?php

require_once(__DIR__ . '/builder.php');

// Squirrel.
Builder::build(array(
	'out' => 'squirrel/squirrel.lib',
	'in'  => array(
		'squirrel/*.cpp',
	),
	'include' => array(
		'squirrel',
	),
	'opts' => '/MD /GF /Ox',
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
	'opts' => '/MD /GF /Ox',
));

// Sqalib.lib
Builder::build(array(
	'out' => 'sqalib/sqalib.lib',
	'in'  => array(
		'sqalib/*.cpp',
	),
	'include' => array(
		'squirrel',
		'sqalib',
	),
	'opts' => '/MD /GF /Ox',
));

// SQ.exe
Builder::build(array(
	'out' => 'sq.exe',
	'in'  => array(
		'sq.c',
		'squirrel/squirrel.lib',
		'sqstdlib/sqstdlib.lib',
	),
	'include' => array(
		'squirrel',
		'sqstdlib',
	),
	'opts' => '/MD /GF /Ox',
));
