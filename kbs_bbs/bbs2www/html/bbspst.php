<?php
	/**
	 * This file lists articles to user.
	 * $Id$
	 */
	require("funcs.php");
	require("boards.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312","","",1);
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else
			html_error_quit("�����������");
		// ����û��ܷ��Ķ��ð�
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0)
			html_error_quit("�����������");
		bbs_set_onboard($brcnum,1);
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0)
			html_error_quit("�����������");
		if(bbs_checkpostperm($usernum, $brdnum) == 0) {
                    if (!strcmp($currentuser["userid"],"guest"))
		      html_error_quit("����ע���ʺ�");
                    else 
		      html_error_quit("�������������������Ȩ�ڴ���������������");
                }
		if (bbs_is_readonly_board($brdarr))
			html_error_quit("������ֻ����������������");
		if (isset($_GET["reid"]))
		{
			$reid = $_GET["reid"];
			if(bbs_is_noreply_board($brdarr))
				html_error_quit("����ֻ�ɷ�������,���ɻظ�����!");
		}
		else {
			$reid = 0;
		}
		settype($reid, "integer");
		$articles = array();
		if ($reid > 0)
		{
			$num = bbs_get_records_from_id($brdarr["NAME"], $reid,$dir_modes["NORMAL"],$articles);
			if ($num == 0)
			{
				html_error_quit("����� Re �ı��");
			}
			if ($articles[1]["FLAGS"][2] == 'y')
				html_error_quit("���Ĳ��ɻظ�!");
		}
		$brd_encode = urlencode($brdarr["NAME"]);
	}
?>
<link rel="stylesheet" type="text/css" href="/ansi.css"/>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="3">
  <tr> 
    <td class="b2">
	    <a href="bbssec.php" class="b2"><font class="b2"><?php echo BBS_FULL_NAME; ?></font></a>
	    -
	    <a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><font class="b2"><?php echo $board; ?></font></a> ��
	    - �������� [ʹ����: <?php echo $currentuser["userid"]; ?>]
	 </td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr><td align="center">

<form name="postform" method="post" action="bbssnd.php?board=<?php echo $brd_encode; ?>&reid=<?php echo $reid; ?>">
<table border="0" cellspacing="5">
<tr>
<td class="b2 sb5">
<?php
		$notes_file = bbs_get_vote_filename($brdarr["NAME"], "notes");
		$fp = FALSE;
		if(file_exists($notes_file))
		{
		    $fp = fopen($notes_file, "r");
		    if ($fp == FALSE)
		    {
    	    	$notes_file = "vote/notes";
                if(file_exists($notes_file))
	    		    $fp = fopen($notes_file, "r");
    		}
		}
		if ($fp == FALSE)
    	{
?>
<font color="green">����ע������: <br />
����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br/></font>
<?php
		}
        else
		{
		    fclose($fp);
			echo bbs_printansifile($notes_file);
		}
?>
</td>
</tr>
<tr><td class="b2 sb5">
������: <?php echo $currentuser["userid"]; ?>, ����: <?php echo $brd_encode; ?> [<a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>">��������</a>]<br>
<?php
		if ($reid)
		{
	        if(!strncmp($articles[1]["TITLE"],"Re: ",4))$nowtitle = $articles[1]["TITLE"];
	        else
	            $nowtitle = "Re: " . $articles[1]["TITLE"];
	    } else {
	        $nowtitle = "";
	    }
?>
��&nbsp;&nbsp;��: <input class="sb1" type="text" name="title" size="40" maxlength="100" value="<?php echo $nowtitle; ?>" /><br />
<?php
		if (bbs_is_attach_board($brdarr))
		{
?>
��&nbsp;&nbsp;��: <input class="sb1" type="text" name="attachname" size="50" value="" disabled="disabled" />
<a href="#" onclick="return GoAttachWindow()" class="b2">��������</a><br />
<?php
		}
?>
ʹ��ǩ���� <select class="sb1" name="signature">
<?php
		if ($currentuser["signum"] == 0)
		{
?>
<option value="0" selected="selected">��ʹ��ǩ����</option>
<?php
		}
		else
		{
?>
<option value="0">��ʹ��ǩ����</option>
<?php
			for ($i = 1; $i <= $currentuser["signum"]; $i++)
			{
				if ($currentuser["signature"] == $i)
				{
?>
<option value="<?php echo $i; ?>" selected="selected">�� <?php echo $i; ?> ��</option>
<?php
				}
				else
				{
?>
<option value="<?php echo $i; ?>">�� <?php echo $i; ?> ��</option>
<?php
				}
			}
		}
?>
</select>
 [<a target="_blank" href="bbssig.php">�鿴ǩ����</a>]
<?php
    if (bbs_is_anony_board($brdarr))
    {
?>
<input type="checkbox" name="anony" value="1" />����
<?php
    }
    if (bbs_is_outgo_board($brdarr)) {
        $local_save = 0;
        if ($reid > 0) $local_save = !strncmp($articles[1]["INNFLAG"], "LL", 2);
?>
<input type="checkbox" name="outgo" value="1"<?php if (!$local_save) echo " checked=\"checked\""; ?> />ת��
<?php
    }
?>
<br />
<textarea class="sb1" name="text"  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.postform.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.postform.submit()' rows="20" cols="80" wrap="physical">
<?php
    if($reid > 0){
    $filename = $articles[1]["FILENAME"];
    $filename = "boards/" . $board . "/" . $filename;
	if(file_exists($filename))
	{
	    $fp = fopen($filename, "r");
        if ($fp) {
		    $lines = 0;
            $buf = fgets($fp,256);       /* ȡ����һ���� ���������µ� ������Ϣ */
			$end = strrpos($buf,")");
			$start = strpos($buf,":");
			if($start != FALSE && $end != FALSE)
			    $quser=substr($buf,$start+2,$end-$start-1);

            echo "\n�� �� " . $quser . " �Ĵ������ᵽ: ��\n";
            for ($i = 0; $i < 3; $i++) {
                if (($buf = fgets($fp,500)) == FALSE)
                    break;
            }
            while (1) {
                if (($buf = fgets($fp,500)) == FALSE)
                    break;
                if (strncmp($buf, "��", 2) == 0)
                    continue;
                if (strncmp($buf, ": ", 2) == 0)
                    continue;
                if (strncmp($buf, "--\n", 3) == 0)
                    break;
                if (strncmp($buf, "\n", 1) == 0)
                    continue;
                if (++$lines > QUOTED_LINES) {
                    echo ": ...................\n";
                    break;
                }
                if (stristr($buf, "</textarea>") == FALSE)  //filter </textarea> tag in the text
                    echo ": ". $buf;
            }
			echo "\n\n";
            fclose($fp);
        }
    }
}
?>
</textarea><br>
<center>
<input class="sb1" type="submit" value="����" />
<input class="sb1" type="reset" value="���" />
<?php
		if (bbs_is_attach_board($brdarr))
		{
?>
<input class="sb1" type="button" name="attach22" value="����" onclick="return GoAttachWindow()" />
<?php
		}
?>
</center>
<script language="JavaScript">
<!--
   function GoAttachWindow(){

   	var hWnd = window.open("bbsupload.php","_blank","width=600,height=300,scrollbars=yes");

	if ((document.window != null) && (!hWnd.opener))

		   hWnd.opener = document.window;

	hWnd.focus();

   	return false;

   }
-->
</script>
</td></tr>
</table></form>

</td></tr>
</table>
<?php
html_normal_quit();
?>
