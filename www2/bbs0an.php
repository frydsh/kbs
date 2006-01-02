<?php
function bbs_ann_display_articles($articles, $isBoard) {
?>
<script type="text/javascript"><!--
var ta = new tabWriter(1,'main wide',0,[['#','5%','center'],['����','7%','center'],['����',0,0],['����','12%','center'],['�༭����','12%','center']]);
<?php
	foreach ($articles as $article) {
		switch($article['FLAG']) {
			case 0:
				$img = 'oldgroup.gif';
				$alt = '����';
				$url = '';
				break;
			case 1:
				$img = 'groupgroup.gif';
				$alt = 'Ŀ¼';
				$url = 'bbs0an.php?path='.rawurlencode($article['PATH']);
				break;
			case 2:
			case 3:
			default:
				$img = 'newgroup.gif';
				$alt = '�ļ�';
				$url = 'bbsanc.php?path='.rawurlencode($article['PATH']);
		}
		$col1 = 'putImageCode(\''.$img.'\',\'alt="'.$alt.'"\')';
		if ($article['FLAG']==3)
			$col2 = '<font color="red">@</font>';
		else
			$col2 = '&nbsp;';
		if ($url)
			$col2 .= '<a href="'.$url.'">'.htmlformat($article['TITLE']).' </a>';
		else
			$col2 .= htmlformat($article['TITLE']).' ';
		$bm = explode(' ',trim($article['BM']));
		$bm = $bm[0];
		$col3 = $bm?'<a href="bbsqry.php?userid='.$bm.'">'.$bm.'</a>':'&nbsp;';
		$col4 = date('Y-m-d',$article['TIME']);
		echo "ta.r($col1,'$col2','$col3','$col4');\n";
	}
?>
ta.t();
//-->
</script>
<?php
	if ($isBoard) echo "</div>"; //dirty way ... for class="doc"
}

require_once('www2-funcs.php');
require_once('www2-board.php');
if (defined ("USE_ROAM")) {
	include_once ('roam_server.php');
	roam_login_init();
}
else
	login_init();

if (isset($_GET['path']))
	$path = trim($_GET['path']);
else 
	$path = "";

if (strstr($path, '.Names') || strstr($path, '..') || strstr($path, 'SYSHome'))
	html_error_quit('�����ڸ�Ŀ¼');

$board = '';
$articles = array();
$path_tmp = '';
$ret = bbs_read_ann_dir($path,$board,$path_tmp,$articles);

switch ($ret) {
	case -1:
		html_error_quit('������Ŀ¼������');
		break;
	case -2:
		html_error_quit('�޷�����Ŀ¼�ļ�');
		break;
	case -3:
		html_error_quit('��Ŀ¼��������');
		break;
	case -9:
		html_error_quit('ϵͳ����');
		break;
	default;
}

$path = $path_tmp;
$isBoard = false;
$up_cnt = bbs_ann_updirs($path,$board,$up_dirs);
if ($board) {
	$brdarr = array();
	if (defined ('USE_ROAM')) {
		$bid = bbs_roam_getboard ($board, $brdarr);
		if ($bid < 0)
			html_error_quit('ϵͳ����');
	}
	else
		$bid = bbs_getboard($board,$brdarr);
	if ($bid) {
		$board = $brdarr['NAME'];
		$usernum = $currentuser['index'];
		if (defined ('USE_ROAM')) {
			$ret = bbs_roam_checkreadperm($usernum, $bid);
			if ( $ret <= 0)
				html_error_quit('�����ڸ�Ŀ¼');
			$ret = bbs_roam_normalboard($board);
			if ( $ret < 0 )
				html_error_quit('ϵͳ����');
			if ( $ret == 1 ) {
				$dotnames = BBS_HOME . '/' . $path . '/.Names';
				if (cache_header('public',filemtime($dotnames),300))
					return;
			}
		}
		else {
			if (bbs_checkreadperm($usernum, $bid) == 0)
				html_error_quit('�����ڸ�Ŀ¼');
			bbs_set_onboard($bid,1);
			if (bbs_normalboard($board)) {
				$dotnames = BBS_HOME . '/' . $path . '/.Names';
				if (cache_header('public',filemtime($dotnames),300))
					return;
			}
		}
		bbs_board_header($brdarr,-1,0);
		$isBoard = true;
?>
<script>
var c = new docWriter('<?php echo addslashes($brdarr["NAME"]); ?>',<?php echo $bid; ?>,0,0,-1,0,0,0,0);
</script>
<?php
	}
	else {
		$board = '';
		bbs_ann_header($board);
	}
	
}
else {
	$dotnames = BBS_HOME . '/' . $path . '/.Names';
	if (cache_header('public',filemtime($dotnames),300))
		return;
	$bid = 0;
	bbs_ann_header();
}

bbs_ann_display_articles($articles, $isBoard);
bbs_ann_xsearch($board);
	
if ($up_cnt >= 2)
	bbs_ann_foot($up_dirs[$up_cnt - 2]);
else
	bbs_ann_foot('');

page_footer();
?>
