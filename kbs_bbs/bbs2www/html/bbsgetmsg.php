<?php
	/**
	 * This file lists articles to user.
	 * $Id$
	 */
    $setboard=0;
    require("funcs.php");
    if ($loginok != 1)
	html_nologin();
    else
    {
        html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            $ret=0;
        else
            $ret=bbs_getwebmsg($srcid,$msgbuf,$srcutmpnum,$sndtime);
?>
<meta http-equiv="pragma" content="no-cache"><style type="text/css">
A {color: #0000FF}
</style>
<?php
        if ($ret)
		{
?>
<bgsound src="/sound/msg.wav">
<body style="BACKGROUND-COLOR: #f0ffd0">
<table width="100%">
<form action=/bbsgetmsg.php name=form0>
  <tr>
    <td valign="top" nowrap="nowrap"><font color="green"><?php echo $srcid; ?></font> (<?php echo strftime("%b %e %H:%M", $sndtime); ?>): </td>
    <td align="left" valign="top"><?php echo htmlspecialchars($msgbuf); ?></td>
    <td align="right" valign="top" nowrap="nowrap"><a target="f3" href="/bbssendmsg.php?destid=<?php 
echo $srcid; ?>&destutmp=<?php 
echo $srcutmpnum; ?>">[��ѶϢ]</a> <a href="bbsgetmsg.php">[����]</a></td>
  </tr>
</form>
</table>
<script language="javascript">if (parent.viewfrm)	parent.viewfrm.rows = "54,*,20";
</script>
<?php
        } else {
		//no msg
?><script language="javascript">if (parent.viewfrm)	parent.viewfrm.rows = "0,*,20";
</script>
<meta http-equiv="Refresh" content="600; url=/bbsgetmsg.php">
<?php
		}
		html_normal_quit();
    }
?>
