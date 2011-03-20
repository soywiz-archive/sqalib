<?php

require_once(__DIR__ . "/builder.php");

//$common = "/MD /GF /Ox /EHsc /DSQUSEDOUBLE=1";
$common = "/MD /GF /Ox /EHsc";

//$squirrel_version = "squirrel2";
$squirrel_version = "squirrel3";

// Squirrel.
Builder::build(array(
	"out" => "{$squirrel_version}/squirrel.lib",
	"in"  => array(
		"{$squirrel_version}/*.cpp",
	),
	"include" => array(
		"squirrel3",
	),
	"opts" => "{$common}",
));

// Sqstdlib.lib
Builder::build(array(
	"out" => "{$squirrel_version}/sqstdlib/sqstdlib.lib",
	"in"  => array(
		"{$squirrel_version}/sqstdlib/*.cpp",
	),
	"include" => array(
		"squirrel3",
		"{$squirrel_version}/sqstdlib",
	),
	"opts" => "{$common}",
));

// Sqalib.lib
Builder::build(array(
	"out" => "sqalib/sqalib.lib",
	"in"  => array(
		"sqalib/*.cpp",
	),
	"include" => array(
		"squirrel3",
		"{$squirrel_version}/sqstdlib",
		"sqalib",
		"sqrat",
	),
	"opts" => "{$common}",
));

// SQ.exe
Builder::build(array(
	"out" => "sq.exe",
	"in"  => array(
		"sq.c",
		"{$squirrel_version}/squirrel.lib",
		"{$squirrel_version}/sqstdlib/sqstdlib.lib",
		"sqalib/sqalib.lib",
	),
	"include" => array(
		"squirrel3",
		"{$squirrel_version}/sqstdlib",
		"sqalib",
		"sqrat",
	),
	"opts" => "{$common}",
));
