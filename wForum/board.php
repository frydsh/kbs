<?php


$setboard=1;

require("inc/funcs.php");
require("inc/user.inc.php");
require("inc/board.inc.php");
require("inc/conn.php");

global $boardArr;
global $boardID;
global $boardName;
global $page;

preprocess();



setStat("�����б�");

show_nav($boardName);

if (isErrFounded()) {
	html_error_quit() ;
} else {
	?>
	<br>
	<TABLE cellSpacing=0 cellPadding=0 width=97% border=0 align=center>
	<?php

	if ($loginok==1) {
		showUserMailbox();
?>
</table>
<?php
	}

	board_head_var($boardArr['DESC'],$boardName,$boardArr['SECNUM']);
?>
	<TABLE cellSpacing=0 cellPadding=0 width=97% border=0 align=center>
<?php
	showAnnounce(); 
?>
</TABLE>
<?php
	if ($boardArr['FLAG'] & BBS_BOARD_GROUP ) {
		showSecs($boardArr['SECNUM'],$boardID,true);
	} else {
?>
<script src="inc/loadThread.js"></script>
<iframe width=0 height=0 src="" id="hiddenframe" name="hiddenframe"></iframe>

<?php
		showBoardStaticsTop($boardArr);
?>
<TABLE cellPadding=1 cellSpacing=1 class=TableBorder1 align=center>
<?php

		showBroadcast($boardID,$boardName);

		showBoardContents($boardID,$boardName,$page);

		boardSearchAndJump($boardName, $boardID);

		showBoardSampleIcons();
?>
</table>
<?php
	}
}

//showBoardSampleIcons();
show_footer();

CloseDatabase();

function preprocess(){
	global $boardID;
	global $boardName;
	global $currentuser;
	global $boardArr;
	global $page;
	if (!isset($_GET['name'])) {
		foundErr("δָ�����档");
		return false;
	}
	$boardName=$_GET['name'];
	$brdArr=array();
	$boardID= bbs_getboard($boardName, $brdArr);
	$boardArr=$brdArr;
	$boardName=$brdArr['NAME'];
	if ($boardID==0) {
		foundErr("ָ���İ��治����");
		return false;
	}
	$usernum = $currentuser["index"];
	if (bbs_checkreadperm($usernum, $boardID) == 0) {
		foundErr("����Ȩ�Ķ�����");
		return false;
	}
	if (!isset($_GET['page'])) {
		$page=-1;
	} else {
		$page=intval($_GET['page']);
	}

	bbs_set_onboard($boardID,1);
	return true;
}
?>