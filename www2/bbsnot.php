<?php
	require("www2-funcs.php");
	login_init();

	$usernum = $currentuser["index"];
	if (isset($_GET["board"]))
		$board = $_GET["board"];
	else 
		html_error_quit("�����������");

	$brdarr = array();
	$brdnum = bbs_getboard($board, $brdarr);
	if ($brdnum == 0)
		html_error_quit("�����������");
	if (bbs_checkreadperm($usernum,$brdnum)==0)
		html_error_quit("�����������");
	$top_file= bbs_get_vote_filename($brdarr["NAME"], "notes");
	$isnormalboard = bbs_normalboard($board);
	if ($isnormalboard) {
		$mt = file_exists($top_file) ? @filemtime($top_file) : time();
		if (cache_header("public",$mt,1800))
			return;
	}

	bbs_board_nav_header($brdarr, "����¼");
	$brd_encode = urlencode($brdarr["NAME"]);
?>
<link rel="stylesheet" type="text/css" href="ansi.css"/>
<script type="text/javascript"><!--
function writeNote()
{
	var bbsnote,notecontent,divbbsnot;
<?php
	$s = false;
	if (file_exists($top_file)) {
		$s = bbs_printansifile($top_file);
	}
	if (!is_string($s))
	{
		echo "\tnotecontent='".addslashes("<br/><br/><br/>&nbsp; &nbsp; &nbsp; &nbsp; �����������ޡ�����¼����")."';\n";
		echo "\tbbsnote='".addslashes("<div class=\"green\">����ע������: <br/>����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����</div>")."';\n";
	} else {
		echo "\tbbsnote=notecontent='".addslashes($s)."';\n";
	}
?>
	if (parent != self && parent && (divbbsnot = parent.document.getElementById('bbsnot')))
	{
		divbbsnot.innerHTML = bbsnote;
	}
	else
	{
		document.getElementById('bbsnot').innerHTML = notecontent;
	}

}
addBootFn(writeNote);
//-->
</script>
<div class="article smaller" id="bbsnot">
</div>
<div class="oper">
[<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>">��������</a>]
<?php
	if (bbs_is_bm($brdnum,$usernum)) {
?>
[<a href="bbsmnote.php?board=<?php echo $brd_encode; ?>">�༭���滭��</a>]
<?php
	}
?> 
[<?php bbs_add_super_fav ('[����¼] '.$brdarr['DESC'], 'bbsnot.php?board='.$brd_encode); ?>]
</div>
<?php
	page_footer();
?>
