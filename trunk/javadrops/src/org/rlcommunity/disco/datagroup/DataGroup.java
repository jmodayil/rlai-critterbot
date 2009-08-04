package org.rlcommunity.disco.datagroup;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.rlcommunity.disco.Drop;
import org.rlcommunity.disco.dropdata.DropArray;
import org.rlcommunity.disco.dropdata.DropData;
import org.rlcommunity.disco.utils.Legend;

public abstract class DataGroup<T> {

    final private DropData[] dropDatas;
    final private Legend legend;
    private final Drop drop;

    public DataGroup(Drop drop) {
	dropDatas = getDropData(drop);
	this.drop = drop;
	legend = getLegend();
    }

    private Legend getLegend() {
	List<String> labels = new ArrayList<String>();
	for (DropData data : dropDatas)
	    labels.add(data.label);
	return new Legend(labels);
    }

    public DropData[] getDropData(Drop drop) {
	List<DropData> datas = new ArrayList<DropData>();
	addDropData(datas, drop.dropDatas());
	DropData[] dropDatas = new DropData[datas.size()];
	datas.toArray(dropDatas);
	return dropDatas;
    }

    private void addDropData(List<DropData> datas, DropData[] dropDatas) {
	for (DropData data : dropDatas)
	    if (data instanceof DropArray)
		addDropData(datas, ((DropArray) data).dropDatas());
	    else if (!data.readOnly && isDataSelected(data))
		datas.add(data);
    }

    public void setWithTime(long time, T... values) {
	drop.setTime(time);
	if (values.length > 0)
	    set(values);
    }

    public void set(T... values) {
	assert values.length == dropDatas.length;
	for (int i = 0; i < values.length; i++)
	    setValue(dropDatas[i], values[i]);
    }

    public void get(T[] values) {
	assert values.length == dropDatas.length;
	for (int i = 0; i < values.length; i++)
	    values[i] = getValue(dropDatas[i]);
    }

    public Legend legend() {
	return legend;
    }

    public Drop drop() {
	return drop;
    }

    @Override
    public boolean equals(Object obj) {
	if (super.equals(obj))
	    return true;
	return drop().equals(((DataGroup<?>) obj).drop());
    }

    abstract protected T getValue(DropData dropData);

    abstract protected boolean isDataSelected(DropData data);

    abstract protected void setValue(DropData dropData, T value);

    public int size() {
	return dropDatas.length;
    }

    @Override
    public String toString() {
	return Arrays.toString(dropDatas);
    }
}
