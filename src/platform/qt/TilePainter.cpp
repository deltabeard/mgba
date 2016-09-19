/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "TilePainter.h"

#include <QImage>
#include <QMouseEvent>
#include <QPainter>

using namespace QGBA;

TilePainter::TilePainter(QWidget* parent)
	: QWidget(parent)
	, m_size(8)
{
	m_backing = QPixmap(256, 768);
	m_backing.fill(Qt::transparent);
	resize(256, 768);
	setTileCount(3072);
}

void TilePainter::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.drawPixmap(QPoint(), m_backing);
}

void TilePainter::resizeEvent(QResizeEvent* event) {
	int calculatedHeight = (m_tileCount * m_size) / (width() / m_size) + m_size / 2;
	if (width() / m_size != m_backing.width() / m_size || m_backing.height() != calculatedHeight) {
		m_backing = QPixmap(width(), calculatedHeight);
		m_backing.fill(Qt::transparent);
	}
}

void TilePainter::mousePressEvent(QMouseEvent* event) {
	int x = event->x() / m_size;
	int y = event->y() / m_size;
	emit indexPressed(y * (width() / m_size) + x);
}

void TilePainter::setTile(int index, const uint16_t* data) {
	QPainter painter(&m_backing);
	int w = width() / m_size;
	int x = index % w;
	int y = index / w;
	QRect r(x * m_size, y * m_size, m_size, m_size);
	QImage tile(reinterpret_cast<const uchar*>(data), 8, 8, QImage::Format_RGB555);
	painter.drawImage(r, tile.rgbSwapped());
	update(r);
}

void TilePainter::setTileCount(int tiles) {
	m_tileCount = tiles;
	setMinimumSize(16, (tiles * m_size) / (width() / m_size));
	resizeEvent(nullptr);
}

void TilePainter::setTileMagnification(int mag) {
	m_size = mag * 8;
	setTileCount(m_tileCount);
}