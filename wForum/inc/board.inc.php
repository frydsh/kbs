<?php

/**
 * Checking whether a board is set with some specific flags or not.
 * 
 * @param $board the board object to be checked
 * @param $flag the flags to check
 * @return TRUE  the board is set with the flags
 *         FALSE the board is not set with the flags
 * @author flyriver
 */
function bbs_check_board_flag($board,$flag)
{
	if ($board["FLAG"] & $flag)
		return TRUE;
	else
		return FALSE;
}

/**
 * Checking whether a board is an anonymous board or not.
 * 
 * @param $board the board object to be checked
 * @return TRUE  the board is an anonymous board
 *         FALSE the board is not an anonymous board
 * @author flyriver
 */
function bbs_is_anony_board($board)
{
	global $BOARD_FLAGS;
	return bbs_check_board_flag($board, $BOARD_FLAGS["ANONY"]);
}

/**
 * Checking whether a board is an outgo board or not.
 * 
 * @param $board the board object to be checked
 * @return TRUE  the board is an outgo board
 *         FALSE the board is not an outgo board
 * @author flyriver
 */
function bbs_is_outgo_board($board)
{
	global $BOARD_FLAGS;
	return bbs_check_board_flag($board, $BOARD_FLAGS["OUTGO"]);
}

/**
 * Checking whether a board is a junk board or not.
 * 
 * @param $board the board object to be checked
 * @return TRUE  the board is a junk board
 *         FALSE the board is not a junk board
 * @author flyriver
 */
function bbs_is_junk_board($board)
{
	global $BOARD_FLAGS;
	return bbs_check_board_flag($board, $BOARD_FLAGS["JUNK"]);
}

/**
 * Checking whether a board is an attachment board or not.
 * 
 * @param $board the board object to be checked
 * @return TRUE  the board is an attachment board
 *         FALSE the board is not an attachment board
 * @author flyriver
 */
function bbs_is_attach_board($board)
{
	global $BOARD_FLAGS;
	return bbs_check_board_flag($board, $BOARD_FLAGS["ATTACH"]);
}

/**
 * Checking whether a board is a readonly board or not.
 * 
 * @param $board the board object to be checked
 * @return TRUE  the board is a readnoly board
 *         FALSE the board is not a readonly board
 * @author flyriver
 */
function bbs_is_readonly_board($board)
{
	global $BOARD_FLAGS;
	return bbs_check_board_flag($board, $BOARD_FLAGS["READONLY"]);
}

function showBoardStaticsTop($boardArr){
?>
<TABLE cellpadding=3 cellspacing=1 class=TableBorder1 align=center><TR><Th height=25 width=100% align=left id=TableTitleLink style="font-weight:normal">���浱ǰ����<b><?php echo $boardArr['CURRENTUSERS'];?></b>�����ߡ���������<?php echo bbs_get_today_article_num($boardArr['NAME'] ); ?></Th></TR></td></tr></TABLE>
<BR>
<table cellpadding=0 cellspacing=0 border=0 width=97% align=center valign=middle><tr><td align=center width=2> </td>
<td align=left style="height:27" valign="center"><table cellpadding=0 cellspacing=0 border=0 ><tr>
<td width="110"><a href=postarticle.php?board=<?php echo $boardArr['NAME']; ?>><div class="buttonClass1" border=0 alt=������></div></a></td>
<td width="110"><a href=# onclick="alert('���������ڿ����У�')"><div class="buttonClass2" border=0 alt=������ͶƱ></div></a></td>
<td width="110"><a href=smallpaper.php?board=<?php echo $boardArr['NAME']; ?>><div class="buttonClass3" border=0 alt=����С�ֱ�></div></a></td>
</tr></table></td>
<td align=right><img src=pic/team2.gif align=absmiddle>
<?php 
	$bms=split(' ',$boardArr['BM']);
	foreach($bms as $bm) {
?>
<a href="dispuser.php?id=<?php echo $bm; ?>" target=_blank title=����鿴�ð�������><?php echo $bm; ?></a>
<?php
	}
?>
</td></tr></table>
<?php
}

function showBoardContents($boardID,$boardName,$page){
	global $dir_modes;
?>
<?php
	$total = bbs_getThreadNum($boardID);
	if ($total<=0) {
?>
<tr><td>
	���滹û������
<td></tr>
</table>
<?php
	} else {
?>
<form action=admin_batch.asp method=post name=batch><TR align=middle>
<Th height=25 width=32 id=TableTitleLink><a href=list.asp?name=<?php echo $boardName; ?>&page=&action=batch>״̬</a></th>
<Th width=* id=TableTitleLink>�� ��  (��<img src=pic/plus.gif align=absmiddle>����չ�������б�)</Th>
<Th width=80 id=TableTitleLink>�� ��</Th>
<Th width=64 id=TableTitleLink>�ظ�</Th>
<Th width=200 id=TableTitleLink>������ | �ظ���</Th></TR>
<?php
		
		$totalPages=ceil($total/ARTICLESPERPAGE);
		if (($page>$totalPages)) {
			$page=$totalPages;
		} else if ($page<1) {
			$page=1;
		}
	/*
		$start=$total-$page* ARTICLESPERPAGE+1;
		$num=ARTICLESPERPAGE;
		if ($start<=0) {
			$num+=$start-1;
			$start=1;
		}
    */
		$start=($page-1)* ARTICLESPERPAGE;
		$num=ARTICLESPERPAGE;

		$articles = bbs_getthreads($boardName, $start, $num,1);
		$articleNum=count($articles);
		for($i=0;$i<$articleNum;$i++){ // ����ط����Կ���ȫ������javascript���������ʾ/�жϵ�load�����ͻ��ˣ�ͬʱ���ٴ����ظ��ַ������紫�� - atppp
			$origin=$articles[$i]['origin'];
			$lastreply=$articles[$i]['lastreply'];
			$threadNum=$articles[$i]['articlenum']-1;

?>
<TR align=middle><TD class=TableBody2 width=32 height=27 align="center">
<?php
//print_r($articles[$i]);
	$upperflag = strtoupper($origin['FLAGS'][0]);
	if ($upperflag=='D') {
		echo "<img src=\"pic/istop.gif\" alt=�̶�������>";
	} elseif( $threadNum > 10 ) {
		echo "<img src=\"pic/blue/hotfolder.gif\" alt=�ظ�����10��>";
	} elseif( ';' == $upperflag ) {
		echo "<img src=\"pic/blue/lockfolder.gif\" alt=����������>";
	} elseif( 'M' == $upperflag || 'B' == $upperflag || 'G' == $upperflag) { //��ʱ�����ɣ�ԭ��ֻ�ж��Ƿ��� 'M' - atppp
		echo "<img src=\"pic/isbest.gif\" alt=������>";
	} else {
		echo "<img src=\"pic/blue/folder.gif\" alt=��������>";
	}
?></TD><TD align=left class=TableBody1 width=* >
<?php 
	if ($threadNum==0) {
		echo '<img src="pic/nofollow.gif" id="followImg'.($i+$start).'">';
	} else {
		echo '<img loaded="no" src="pic/plus.gif" id="followImg'.($origin['ID']).'" style="cursor:hand;" onclick="loadThreadFollow(\''.($origin['ID'])."','".$boardName."')\" title=չ�������б�>";
	}
?><a href="disparticle.php?boardName=<?php echo $boardName ;?>&ID=<?php echo $origin['ID'];?>" title="<?php echo htmlspecialchars($origin['TITLE'],ENT_QUOTES) ;?> <br>���ߣ�<?php echo $origin['OWNER'] ;?><br>������<?php echo strftime("%Y-%m-%d %H:%M:%S", $origin['POSTTIME']); ?>"><?php echo htmlspecialchars($origin['TITLE']) ;?> </a> 
<?php
	$threadPages=ceil(($threadNum+1)/THREADSPERPAGE);
	if ($threadPages>1) {
		echo "<b>[<img src=\"pic/multipage.gif\"> ";
		for ($t=1; ($t<7) && ($t<=$threadPages) ;$t++) {
			echo "<a href=\"disparticle.php?boardName=".$boardName."&ID=".$origin['ID']. "&start=".($t-1)*THREADSPERPAGE."\">".$t."</a> ";
		}
		if ($threadPages>7) {
			if ($threadPages>8) {
				echo "...";
			}
			echo "<a href=\"disparticle.php?boardName=".$boardName."&ID=".$origin['ID']. "&start=".($threadPages-1)*THREADSPERPAGE."\">".$threadPages."</a> ";
		}
		echo " ]</b>";
	}
	if ((($origin['FLAGS'][0] >= 'A') && ($origin['FLAGS'][0] <= 'Z')) || ($origin['FLAGS'][0] == '*')) {
		 //very dirty way, ���Զ��ѣ�ֻ�ж�ԭ���Ƿ�δ�������Ի��ɱȷ��ж�24Сʱ֮�������� blah blah... - atppp
		echo "<img src=\"pic/topnew2.gif\" alt=\"ԭ��δ��\">";
	}
?>
</TD>
<TD class=TableBody2 width=80 align="center"><a href="dispuser.php?id=<?php echo $origin['OWNER'] ;?>" target=_blank><?php echo $origin['OWNER'] ;?></a></TD>
<TD class=TableBody1 width=64 align="center"><?php echo $threadNum; ?></TD>
<TD align=left class=TableBody2 width=200>&nbsp;<a href="disparticle.php?boardName=<?php echo $boardName ;?>&ID=<?php echo $origin['ID'];?>&start=<?php echo $total; ?>">
<?php
			echo strftime("%Y-%m-%d %H:%M", $lastreply['POSTTIME']);
?></a>&nbsp;<font color=#FF0000>|</font>&nbsp;<a href=dispuser.php?id=<?php echo $lastreply['OWNER']; ?>  target=_blank>
<?php 
			echo $lastreply['OWNER'];
?></a></TD></TR>
<?php
			if ($threadNum>0) {
?>
<tr style="display:none" id="follow<?php echo $origin['ID']; ?>"><td colspan=5 id="followTd<?php echo $origin['ID'];?>" style="padding:0px"><div style="width:240px;margin-left:18px;border:1px solid black;background-color:lightyellow;color:black;padding:2px" onclick="loadThreadFollow('<?php echo ($origin['ID']);?>','<?php echo $boardName; ?>')">���ڶ�ȡ���ڱ�����ĸ��������Ժ��</div></td></tr>
<?php
			}
		}
?>
</form></table><table border=0 cellpadding=0 cellspacing=3 width=97% align=center >
<form method=get action="board.php">
<input type="hidden" name="name" value="<?php echo $boardName ; ?>">
<tr><td valign=middle>ҳ�Σ�<b><?php echo $page; ?></b>/<b><?php echo $totalPages; ?></b>ҳ ÿҳ<b><?php echo ARTICLESPERPAGE; ?></b> ������<b><?php echo $total ?></b></td><td valign=middle ><div align=right >��ҳ��
<?php
    $lastTenPages=(floor(($page-1)/ 10))*10;
	if ($page==1) {
		echo "<font face=webdings color=\"#FF0000\">9</font>   "; //ToDo: XHTML ������ʹ�� webdings ���塣
	}   else {
		echo "<a href=\"board.php?name=".$boardName."&page=1\" title=��ҳ><font face=webdings>9</font></a>   ";
	} 

	if ($lastTenPages>0)  {
		echo "<a href='?name=". $boardName ."&page=" . $lastTenPages . "' title=��ʮҳ><font face=webdings>7</font></a>   ";  
	} 

	echo "<b>";
	for ($i=$lastTenPages+1; $i<=$lastTenPages+10; $i++) {
		if ($i==$page)	{
			echo "<font color=#ff0000>".$i."</font> ";
		} else {
			echo "<a href='board.php?name=".$boardName."&page=".$i."'>".$i."</a> ";
		} 
		if ($i==$totalPages) {
		  break;
		} 
	} 
	echo "</b>";
	if ($i<$totalPages) {
		echo "<a href='board.php?name=".$boardName."&page=".$i."' title=��ʮҳ><font face=webdings>8</font></a>   ";  
	} 
	if ($page==$totalPages) {
		echo "<font face=webdings color=#ff0000>:</font>   ";
	}  else  {
		echo "<a href='board.php?name=".$boardName."&page=".$totalPages."' title=βҳ><font face=webdings>:</font></a>   ";
	} 
?>
ת��:<input type=text name="page" size=3 maxlength=10  value=1><input type=submit value=Go ></div></td></tr>
</form></table>
<?php
	}
}

function showBroadcast($boardID,$boardName){
	global $conn;
?>
<tr><td class=TableBody1 colspan=5 height=20>
	<table width=100% ><tr><td valign=middle height=20 width=50><a href=allpaper.php?board=<?php echo $boardName; ?> title=����鿴����̳����С�ֱ�><b>�㲥</b></a>��</td><td width=*> <marquee scrolldelay=150 scrollamount=4 onmouseout="if (document.all!=null){this.start()}" onmouseover="if (document.all!=null){this.stop()}">
<?php
	$sth = $conn->query("SELECT ID,Owner,Title FROM smallpaper_tb where Addtime>=subdate(Now(),interval 1 day) and boardID=" . $boardID . " ORDER BY Addtime desc limit 5");
	while($rs = $sth->fetchRow(DB_FETCHMODE_ASSOC)) {
		print "����<font color=#ff0000>".$rs['Owner']."</font>˵��<a href=javascript:openScript('viewpaper.php?id=".$rs['ID']."&boardname=".$boardName."',500,400)>".htmlspecialchars($rs['Title'],ENT_QUOTES)."</a>";
  } 
  unset($rs);
  $sth->free();
  /* ToDo: ע�⣺���� link ��ʱ�������������о����������ġ�ע����������û�з�ҳ��- atppp */
?>
	</marquee><td align=right width=240><a href="queryresult.php?boardName=<?php echo $boardName; ?>&title=&title2=&title3=&userid=&dt=0&mg=on" title=�鿴���澫����><font color=#FF0000><B>����</B></font></a>
	| <a href=# onclick="alert('���������ڿ����У�')" title=�鿴����������ϸ���>����</a>
	| <a href=# onclick="alert('���������ڿ����У�')" title=�鿴�����¼�>�¼�</a>
	| <a href=# onclick="alert('���������ڿ����У�')" title=�鿴�����û���Ȩ��>Ȩ��</a>
    | <a href=# onclick="alert('���������ڿ����У�')">����</a></td></tr></table>
</td></tr>
<?php
}

function board_head_var($boardDesc,$boardName,$secNum)
{
  GLOBAL $SiteName;
  GLOBAL $SiteURL;
  GLOBAL $stats;
  global $section_names;
  if ($URL=='') {
	  $URL=$_SERVER['PHP_SELF'];
  }
?>
<table cellspacing=1 cellpadding=3 align=center class=TableBorder2>
<tr><td>
<img src="pic/forum_nav.gif"> <a href="index.php"><?php   echo $SiteName; ?></a> �� 
<a href="section.php?sec=<?php echo $secNum; ?>"><?php echo $section_names[intval($secNum)][0] ; ?></a> �� <a href="board.php?name=<?php echo $boardName; ?>"><?php echo $boardDesc; ?></a> �� <?php echo $stats; ?> 
</td></tr>
</table>
<br>
<?php 
} 
function boardJump(){
	global $section_names;
	global $sectionCount;
	global $section_nums;
	global $yank;
?>
<div align=right><select onchange="if(this.options[this.selectedIndex].value!=''){location=this.options[this.selectedIndex].value;}">
<option selected>��ת��̳��...</option>
<?php
	for ($i=0;$i<$sectionCount;$i++){
		echo "<option value=\"section.php?sec=".$i."\">��".$section_names[$i][0]."</option>";
		$boards = bbs_getboards($section_nums[$i], 0, $yank);
		if ($boards != FALSE) {
			$brd_desc = $boards["DESC"]; // ��������
			$brd_name = $boards["NAME"];
			$rows = sizeof($brd_desc);
			for ($t = 0; $t < $rows; $t++)	{
				echo "<option value=\"board.php?name=".$brd_name[$t]."\">&nbsp;&nbsp;��".$brd_desc[$t]."</option>";
			}
		}
	}
?>
</select></div>
<?php
}
function boardSearchAndJump($boardName, $boardID){

?>
<table border=0 cellpadding=0 cellspacing=3 width=97% align=center>
<tr>
<FORM METHOD=GET ACTION="queryresult.php">
<input type="hidden" name="boardName" value="<?php echo $boardName; ?>">
<td width=50% valign=middle nowrap height=40>����������<input type=text name=title>&nbsp;<input type=submit name=submit value=����></td>
</FORM>
<td valign=middle nowrap width=50% > 
<?php
	boardJump();
?>
</td></tr></table><BR>
<?php
}

function showBoardSampleIcons(){
	global $SiteName;
?>
<table cellspacing=1 cellpadding=3 width=100% class=TableBorder1 align=center><tr><th width=80% align=left>��-=> <?php echo $SiteName; ?>ͼ��</th><th noWrap width=20% align=right>����ʱ���Ϊ - <?php echo SERVERTIMEZONE; ?> &nbsp;</th></tr><tr><td colspan=2 class=TableBody1><table cellspacing=4 cellpadding=0 width=92% border=0 align=center><tr><td><img src=pic/blue/folder.gif> ���ŵ�����</td><td><img src=pic/blue/hotfolder.gif> �ظ�����10��</td><td><img src=pic/blue/lockfolder.gif> ����������</td><td><img src=pic/istop.gif> �̶������� </td><td><img src=pic/ztop.gif> �̶ܹ������� </td><td> <img src=pic/isbest.gif> �������� </td><td> <img src=pic/closedb.gif> ͶƱ���� </td></tr></table></td></tr></table>
<?php
}
?>