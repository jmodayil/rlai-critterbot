package org.rlcommunity.disco.datagroup;

import junit.framework.Assert;

import org.junit.Test;
import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.dropdata.DropArray;
import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.dropdata.DropTime;
import org.rlcommunity.disco.dropdata.DropVar;
import org.rlcommunity.disco.utils.Legend;

public class DropTest {
    final static public DropData[] dropData01 = { new DropVar("Data1") };
    final static public DropData[] dropData02 = { new DropTime(),
	    new DropVar("Data1") };
    final static public DropData[] dropData03 = { new DropVar("Data1"),
	    new DropVar("Data2", 9), new DropVar("Data3") };
    final static public DropData[] dropData04 = { new DropVar("Data1"),
	    new DropVar("Data2"), new DropVar("Data3") };
    final static public DropData[] dropData05 = { new DropArray("Bla", 2),
	    new DropVar("Data"), new DropArray("Bl", "i", "o") };

    @Test
    public void testDropSize() {
	Assert.assertEquals(1 * 4, new Drop("a", dropData01).size);
	Assert.assertEquals(3 * 4, new Drop("ab", dropData02).size);
	Assert.assertEquals(5 * 4, new Drop("a", dropData05).size);
    }

    static protected DataGroup<Integer> newDrop(DropData[] dropDatas) {
	return newDrop(dropDatas, (Integer[]) null);
    }

    static protected DropVarGroup newDrop(DropData[] dropDatas,
	    Integer... values) {
	return newDrop("", dropDatas, values);
    }

    static protected DropVarGroup newDrop(String dropName,
	    DropData[] dropDatas, Integer... values) {
	Drop drop = new Drop(dropName, dropDatas);
	DropVarGroup group = new DropVarGroup(drop);
	if (values != null)
	    group.set(values);
	return group;
    }

    @Test
    public void testDropLegend() {
	Legend legend = newDrop(dropData05).legend();
	Assert.assertEquals(0, legend.indexOf("Bla0"));
	Assert.assertEquals(1, legend.indexOf("Bla1"));
	Assert.assertEquals(2, legend.indexOf("Data"));
	Assert.assertEquals(3, legend.indexOf("Bli"));
	Assert.assertEquals(4, legend.indexOf("Blo"));
    }

    @Test
    public void testEquals() {
	DataGroup<Integer> drop01 = newDrop(dropData01, 1);
	DataGroup<Integer> drop02 = newDrop(dropData01, 1);
	Assert.assertEquals(drop01, drop01);
	Assert.assertEquals(drop01, drop02);

	drop02.set(2);
	Assert.assertFalse(drop01.equals(drop02));
    }

    @Test
    public void testDiffFromName() {
	DataGroup<Integer> drop01 = newDrop("a", dropData01, 1);
	DataGroup<Integer> drop02 = newDrop("b", dropData01, 1);
	Assert.assertFalse(drop01.equals(drop02));
    }

    @Test
    public void testEqualsWithTime() {
	DataGroup<Integer> drop01 = newDrop(dropData02);
	drop01.setWithTime(0, 1);
	DropVarGroup drop02 = newDrop(dropData02, 1);
	drop02.setWithTime(0, 1);
	Assert.assertEquals(drop01, drop02);

	drop02.setWithTime(2);
	Assert.assertFalse(drop01.equals(drop02));

	drop02.setWithTime(0, 1);
	Assert.assertEquals(drop01, drop02);

	drop02.setWithTime(0, 2);
	Assert.assertFalse(drop01.equals(drop02));

	drop02.toString();
    }

    @Test
    public void testEqualsWithFinal() {
	DropVarGroup drop01 = newDrop(dropData03, 1, 2);
	DropVarGroup drop02 = newDrop(dropData04, 1, 9, 2);
	Assert.assertEquals(drop01, drop02);
	drop01.toString();
	drop02.toString();
    }

    @Test
    public void testEqualsWithArray() {
	DropVarGroup drop01 = newDrop(dropData05, 1, 2, 3, 4, 5);
	DropVarGroup drop02 = newDrop(dropData05, 1, 2, 3, 4, 5);
	Assert.assertEquals(drop01, drop02);
	drop01.toString();
	drop02.set(1, 2, 3, 4, 6);
	Assert.assertFalse(drop01.equals(drop02));
    }

}
