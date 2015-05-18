function putHeader(title, desc)
{
document.write('<table width="600" border="0" cellspacing="0" cellpadding="0">');
document.write('	<tr><td width="16">&nbsp;</td>');
document.write('	<td><span class="pt">');
document.write(title);
document.write('	</span></td></tr>');
document.write('	<tr><td width="16">&nbsp;</td>');
document.write('	<td><span class="ph">');
document.write(desc);
document.write('	</span></td></tr>');
document.write('	<tr colspan="3"><td height="17">&nbsp;</td></tr>');
document.write('	<tr><td width="16">&nbsp;</td>');
}

function startArea()
{
document.write('    	<td>');
document.write('	<table width="100%" class="fb" border="0" cellspacing="0" cellpadding="0" align="center">');
document.write('	<tr>');
document.write('                  <td valign="top" height="10" width="10"><img src="images/border_tl.gif" align="top" border="0" width="10" height="10"></td>');
document.write('                  <td valign="top" height="10">&nbsp;</td>');
document.write('                  <td valign="top" height="10" width="10"><img src="images/border_tr.gif" align="top" border="0" width="10" height="10"></td>');
document.write('     	</tr>');
document.write('	<tr>');
document.write('		<td height="10" width="10"><img src="images/clearpixel.gif" border="0" width="1" height="1"></td>');
document.write('		<td height="10">');
}

function endArea()
{
document.write('      		</td>');
document.write('      		<td height="10" width="10"><img src="images/clearpixel.gif" border="0" width="1" height="1"></td>');
document.write('	</tr>');
document.write('	<tr>');
document.write('                  <td valign="bottom" height="10" width="10"><img src="images/border_bl.gif" align="bottom" border="0" width="10" height="10"></td>');
document.write('                  <td valign="bottom" height="10">&nbsp;</td>');
document.write('                  <td valign="bottom" height="10" width="10"><img src="images/border_br.gif" align="bottom" border="0" width="10" height="10"></td>');
document.write('      	</tr>');
document.write('      	</table>');
document.write('    	</td>');
}

function skipArea()
{
endArea();

document.write( "</tr><tr><td>&nbsp;</td>");
document.write( "<td>&nbsp;</td>");

document.write('        </tr><tr>');
document.write('<table width="600" border="0" cellspacing="0" cellpadding="0">');
document.write('        <tr><td width="16">&nbsp;');

startArea();

}

function putFooter()
{
document.write('     	<td width="16">&nbsp;</td>');

document.write('	</tr>');
document.write('</table>');
document.write('<br>');
}
function putSubtitle(subtitle)
{
document.write(		'<tr valign="top"><td class="lh" colspan=2>');
document.write(subtitle);
document.write(		'</td></tr>');
document.write(		'<tr valign="top"><td class="r" colspan=2></td></tr>');
document.write(		'<tr><td colspan="2"><hr size="1" width="100%" noshade color=#B8B889></td></tr>');
}
