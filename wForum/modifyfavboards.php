<?php
require("inc/funcs.php");

require("inc/usermanage.inc.php");

require("inc/user.inc.php");

setStat("�����ղذ���");

show_nav();

if ($loginok==1) {
	showUserMailbox();
	head_var($userid."�Ŀ������","usermanagemenu.php",0);
	showUserManageMenu();
	main();
} else {
	foundErr("��ҳ��Ҫ������ʽ�û����ݵ�½֮����ܷ��ʣ�");
}

if (isErrFounded()) {
	html_error_quit();
}

show_footer();

function showBoardGroup($secNum, $group, $boardName, $boardDesc = "") {
	global $section_nums;
	global $yank;
?>
<tr><th class=TableBody1 align=left valign=middle>
<?php
	echo $boardName; 
	if ($boardDesc != "") echo " (".$boardDesc.")";
?></th></tr>
<tr><td class=TableBody2 align=left valign=middle>
<?php
		$boards = bbs_getboards($section_nums[$secNum], $group, $yank);
		if ($boards != FALSE) {
			$brd_desc = $boards["DESC"]; // ��������
			$brd_name = $boards["NAME"];
			$brd_flag = $boards["FLAG"];
			$brd_id   = $boards["BID"];
			$rows = sizeof($brd_desc);
?>
<table cellpadding=3 cellspacing=1 align=center class=TableBorder2>
<?php
			$boards = 0;
			for ($t = 0; $t < $rows; $t++)	{
				if (!($brd_flag[$t] & BBS_BOARD_GROUP)) {
					if ($boards % 3 == 0) echo "<tr>";
?>
<td width=33% align=left>
<input type="checkbox" value="1"  name="<?php echo $brd_name[$t]; ?>"<?php if (bbs_is_favboard($brd_id[$t])) echo " checked=\"checked\""; ?>>
<a href="board.php?name=<?php echo $brd_name[$t]; ?>"><?php echo $brd_name[$t]; ?> (<?php echo $brd_desc[$t]; ?>)</a></td>
<?php
					$boards++;
					if ($boards % 3 == 0) echo "</tr>";
				}
			}
			if ($boards % 3 != 0) {
				for (; $boards % 3 != 0; $boards++) echo "<td width=33%> </td>";
				echo "</tr>";
			}
?>
</table>
<?php
			for ($t = 0; $t < $rows; $t++)	{
				if ($brd_flag[$t] & BBS_BOARD_GROUP ) {
?>
<table cellpadding=3 cellspacing=1 align=center class=TableBorder1>
<?php
					showBoardGroup($secNum, $brd_id[$t], $brd_name[$t], $brd_desc[$t]);
?>
</table>
<?php
				}
			}
		}
?>
</td></tr>
<?php	
}

function main() {
	global $section_names;
	global $sectionCount;
	if(bbs_load_favboard(0)==-1) { //0: always use the top level fav boards
		foundErr("�޷���ȡ�ղؼ�");
		return false;
	}
?>
<br>
<base target="_blank" />
<form method="post" target="_self" action="savefavboards.php">
<table cellpadding=3 cellspacing=1 align=center class=TableBorder1>
<?php
	for ($i=0;$i<$sectionCount;$i++){
		showBoardGroup($i, 0, $section_names[$i][0]);
	}
?>
</table>
<p align="center"><input type="submit" value="���浽�ղؼж���Ŀ¼" /></p>
</form>
<?php
	return true;
}
?>