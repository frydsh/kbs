<?php
	/**
	 * This file post the article to the board.
	 * $Id$
	 */
	 
	require("funcs.php");
	require("boards.php");
		
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
		if( !isset( $_GET["board"] )) {
			html_error_quit("δָ�����İ���!");
		}
		$boardName = $_GET["board"];
		$brdArr=array();
		$boardID= bbs_getboard($boardName,$brdArr);
		if( $boardID == 0) html_error_quit("ָ���İ��治����!");
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $boardID) == 0) html_error_quit("����Ȩ�Ķ��ð�!");
		if (bbs_is_readonly_board($boardArr)) html_error_quit("����Ϊֻ��������!");
		if (bbs_checkpostperm($usernum, $boardID) == 0) html_error_quit("����Ȩ�ڸð��淢��!");
		
		if (!isset($_POST["title"])) html_error_quit("û��ָ�����±���!");
		if (!isset($_POST["text"])) html_error_quit("û��ָ����������!");
		if (!isset($_POST["reid"])) $reID = $_GET["reid"];   
		else
			$reID = 0;
		if (bbs_is_outgo_board($brdArr)) $outgo = intval($_POST["outgo"]);
		else $outgo = 0;
		
		settype($reID, "integer");
				
		$articles = array();
		if ($reID > 0) {
			$num = bbs_get_records_from_id($boardName, $reID, $dir_modes["NORMAL"], $articles);
			if ($num == 0) html_error_quit("�����Re�ı��!");
			if ($articles[1]["FLAGS"][2] == 'y') html_error_quit("���Ĳ��ɻظ�!");
		}
		
		//post articles
		$ret = bbs_postarticle($boardName, preg_replace("/\\\(['|\"|\\\])/","$1",$_POST["title"]), 
			preg_replace("/\\\(['|\"|\\\])/","$1",$_POST["text"]), intval($_POST["signature"]), $reID, 
			$outgo, intval($_POST["anony"]));
		switch ($ret) {
			case -1:
				html_error_quit("���������������!");
				break;
			case -2: 
				html_error_quit("����Ϊ����Ŀ¼��!");
				break;
			case -3: 
				html_error_quit("����Ϊ��!");
				break;
			case -4: 
				html_error_quit("����������Ψ����, ����������Ȩ���ڴ˷�������!");
				break;		
			case -5:	
				html_error_quit("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��!");
				break;	
			case -6:
				html_error_quit("���η��ļ������,����Ϣ��������!");	
				break;
			case -7: 
				html_error_quit("�޷���ȡ�����ļ�! ��֪ͨվ����Ա, лл! ");
				break;
			case -8:
				html_error_quit("���Ĳ��ɻظ�!");
				break;
			case -9:
				html_error_quit("ϵͳ�ڲ�����, ��Ѹ��֪ͨվ����Ա, лл!");
				break;
		}
?>		
<table cellpadding=3 cellspacing=1 align=center class=TableBorder1>
<tr align=center><th width="100%">���ĳɹ���</td>
</tr><tr><td width="100%" class=TableBody1>
��ҳ�潫��3����Զ����ذ��������б�<meta HTTP-EQUIV=REFRESH CONTENT='3; URL=bbsdoc.php?board=<?php echo $boardName; ?>' >��<b>������ѡ�����²�����</b><br><ul>
<li><a href="<?php echo MAINPAGE_FILE; ?>">������ҳ</a></li>
<li><a href="bbsdoc.php?board=<?php   echo $boardName; ?>">����<?php   echo $boardArr['DESC']; ?></a></li>
</ul></td></tr></table>

<?php
	html_normal_quit();
	}
?>	
