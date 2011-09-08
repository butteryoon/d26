#!/usr/bin/perl

use warnings;
use strict;

my $line;
my ($temp_write, $read, @write, $write_cnt);
my $proc = "./process_list.conf";
my $input = "./flow_list.conf";
my $gifo = "./gifo.conf";
my $cifo = "./cifo.conf";
my $chid = 0;
my $grid = 0;
my $cellCnt = 1000000;
my $cellSize = 8;
my $wBufCnt = 1;
my $rBufCnt = 1;
my $wSemFlag = 0;
my $wSemKey = 20000;
my $rSemFlag = 0;
my $rSemKey = 10000;
my $uiShmKey = 10314;
my $uiHeadRoomSize = 5242880;
my (%write_matrix, %read_matrix);
my $key;
my %process;

open PROC, "$proc" || die $!;
while($line = <PROC>)
{
	chop($line);
	if($line =~ /(\S+)(\s*)=(\s*)(\d+)/)
	{
		print "PROCESS $1\t$4\n";
		$process{"$1"}="$4";
	}
}

open GIFO, ">$gifo" || die $!;
print GIFO "#### group setting ####\n";
print GIFO "#grId/chId(, chId)/#\n";
open CIFO, ">$cifo" || die $!;
print CIFO "#### st_CIFOCONF setting ####\n";
print CIFO "uiShmKey = $uiShmKey         # INC/ipaf_define.h S_SSHM_CIFO_MEM 0x284A #\n";
print CIFO "uiHeadRoomSize = $uiHeadRoomSize   # gifo size #\n";
print CIFO "\n";
print CIFO "#### st_CHANCONF setting ####\n";
print CIFO "#chId/cellCnt/cellSize/wBufCnt/rBufCnt/wSemflag/wSemKey/rSemFlag/rSemKey/#\n";
open INPUT, "$input" || die $!;
while($line = <INPUT>)
{
	chop($line);
	if($line =~ /([0-9a-zA-Z=_(), \t]+)(\s*)->(\s*)(\S+)/)
	{
		$temp_write = $1;
		$read = $4;
		print "t1=[$temp_write]\tt2=[$read]\n";
		@write = split(/,/, $temp_write);
		$write_cnt = @write;
		$rBufCnt = $write_cnt;
		print "write_cnt=$write_cnt\n";
		print GIFO "$grid/";

		for(my $i = 0; $i < $write_cnt; $i++)
		{
			$cellCnt = 1000000;
			$wSemFlag = 0;
			$rSemFlag = 0;
			print "==> TTT=$write[$i]\n";
			if($write[$i] =~ /cellCnt(\s*)=(\s*)(\d+)/)
			{
				$cellCnt=$3;
				print "==@ cellCnt=$cellCnt\n";
			}
			if($write[$i] =~ /wSemFlag(\s*)=(\s*)(\d+)/)
			{
				$wSemFlag=$3;
				print "==@ wSemFlag=$wSemFlag\n";
			}
			if($write[$i] =~ /rSemFlag(\s*)=(\s*)(\d+)/)
			{
				$rSemFlag=$3;
				print "==@ rSemFlag=$rSemFlag\n";
			}
			if($write[$i] =~ /([0-9a-zA-Z_]+)/)
			{
				$write[$i] = $1;
			}
			print "==> TTK=$write[$i]\n";

			print CIFO "$chid/$cellCnt/$cellSize/$wBufCnt/$rBufCnt/$wSemFlag/$wSemKey/$rSemFlag/$rSemKey/\t# $write[$i] - $read #\n";
			if($i == $write_cnt - 1)
			{
				print GIFO "$chid/\t# $read #\n";
			}
			else
			{
				print GIFO "$chid,";
			}

			if(!exists($process{$write[$i]}))
			{
				print "!!!!!!! INVALID PROCESS NAME=$write[$i]\n";
				exit;
			}
			if(!exists($process{$read}))
			{
				print "!!!!!!! INVALID PROCESS NAME=$read\n";
				exit;
			}

			$write_matrix{"$process{$write[$i]}/$process{$read}/$chid/\t# $write[$i]/$read/ #"}++;
			$read_matrix{"$process{$read}/$grid/\t# $read #"}++;
			$chid++;
			$wSemKey++;
			$rSemKey++;
		}
		$grid++;
	}
}

print GIFO "END_GROUP#\n";

print GIFO "#### write matrix setting ####\n";
print GIFO "#wProcSeq/rProcSeq/chId/#\n";
foreach $key (keys %write_matrix)
{
	print GIFO "$key\n";
}
print GIFO "END_WRITE_MATRIX#\n";

print GIFO "#### read matrix setting ####\n";
print GIFO "#procSeq/grId/#\n";
foreach $key (keys %read_matrix)
{
	print GIFO "$key\n";
}
print GIFO "END_READ_MATRIX#\n";

