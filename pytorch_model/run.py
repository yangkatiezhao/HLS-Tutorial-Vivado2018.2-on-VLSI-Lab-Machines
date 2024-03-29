from __future__ import print_function
import sys

import time
from torchvision import datasets, transforms
from dataset import customDataset
from utils import *
from models import *
import numpy as np
import xml.etree.ElementTree as ET

def ParseXml(xml_path):
    if not os.path.exists(xml_path):
        return [0, 0, 0, 0]
    tree = ET.parse(xml_path)
    root = tree.getroot()
    obj = root.find('object')
    label = obj.find('name').text
    xmin = int(float(obj.find('bndbox').find('xmin').text))
    ymin = int(float(obj.find('bndbox').find('ymin').text))
    xmax = int(float(obj.find('bndbox').find('xmax').text))
    ymax = int(float(obj.find('bndbox').find('ymax').text))
    return [xmin, ymin, xmax, ymax]

def GetIOU(bbox1, bbox2):
    from utils import bbox_iou
    b1 = torch.Tensor(bbox1).unsqueeze(0)
    b2 = torch.Tensor(bbox2).unsqueeze(0)
    iou = bbox_iou(b1, b2, x1y1x2y2=True)
    return iou.item()

def main():
    imgDir = './samples/'
    rootDir = './result/'
    if not os.path.isdir(rootDir):
        os.mkdir(rootDir)
        os.mkdir(os.path.join(rootDir, 'xml'))
    myXmlDir = './result/xml/'
    if not os.path.isdir(myXmlDir):
        os.mkdir(myXmlDir)
    timeDir = './result/time/'
    if not os.path.isdir(timeDir):
        os.mkdir(timeDir)
    allTimeFile = './result/time/alltime.txt'
    gdXmlDir = './gd/'

    modeltype = 'DetNet()'
    weightfile = 'dac.weights'

    num_workers = 10
    batch_size = 4

    model = eval(modeltype)
    region_loss = model.loss
    load_net(weightfile, model)
    region_loss.seen = model.seen

    init_width = model.width
    init_height = model.height
    kwargs = {'num_workers': num_workers, 'pin_memory': True}
    cdataset = customDataset(imgDir, shape=(init_width, init_height),
                             transform=transforms.Compose([
                                 transforms.ToTensor(),
                                 transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.25, 0.25, 0.25]),
                             ]))
    test_loader = torch.utils.data.DataLoader(
        cdataset,
        batch_size=batch_size,
        shuffle=False,
        **kwargs)
    # model = model.cuda()

    model.eval()
    cur_model = model

    anchors = cur_model.anchors
    num_anchors = cur_model.num_anchors
    anchor_step = len(anchors) // num_anchors
    h = 20
    w = 40
    total = 0
    imageNum = cdataset.__len__()
    results = np.zeros((imageNum, 4))

    grid_x = torch.linspace(0, w - 1, w).repeat(h, 1).repeat(batch_size * num_anchors, 1, 1).view(
        batch_size * num_anchors * h * w)
    grid_y = torch.linspace(0, h - 1, h).repeat(w, 1).t().repeat(batch_size * num_anchors, 1, 1).view(
        batch_size * num_anchors * h * w)
    anchor_w = torch.Tensor(anchors).view(num_anchors, anchor_step).index_select(1, torch.LongTensor([0]))
    anchor_h = torch.Tensor(anchors).view(num_anchors, anchor_step).index_select(1, torch.LongTensor([1]))
    anchor_w = anchor_w.repeat(batch_size, 1).repeat(1, 1, h * w).view(batch_size * num_anchors * h * w)
    anchor_h = anchor_h.repeat(batch_size, 1).repeat(1, 1, h * w).view(batch_size * num_anchors * h * w)
    sz_hw = h * w
    sz_hwa = sz_hw * num_anchors

    stime = time.time()
    for batch_idx, data in enumerate(test_loader):
        # data = data.cuda()
        output = model(data).data
        batch = output.size(0)
        output = output.view(batch * num_anchors, 5, h * w).transpose(0, 1).contiguous().view(5,
                                                                                              batch * num_anchors * h * w)

        if batch != batch_size:
            # print("Last batch")
            grid_x = torch.linspace(0, w - 1, w).repeat(h, 1).repeat(batch * num_anchors, 1, 1).view(
                batch * num_anchors * h * w)
            grid_y = torch.linspace(0, h - 1, h).repeat(w, 1).t().repeat(batch * num_anchors, 1, 1).view(
                batch * num_anchors * h * w)
            anchor_w = torch.Tensor(anchors).view(num_anchors, anchor_step).index_select(1, torch.LongTensor([0]))
            anchor_h = torch.Tensor(anchors).view(num_anchors, anchor_step).index_select(1, torch.LongTensor([1]))
            anchor_w = anchor_w.repeat(batch, 1).repeat(1, 1, h * w).view(batch * num_anchors * h * w)
            anchor_h = anchor_h.repeat(batch, 1).repeat(1, 1, h * w).view(batch * num_anchors * h * w)

        det_confs = torch.sigmoid(output[4])
        det_confs = convert2cpu(det_confs)

        for b in range(batch):
            det_confs_inb = det_confs[b * sz_hwa:(b + 1) * sz_hwa].numpy()
            ind = np.argmax(det_confs_inb)

            xs_inb = torch.sigmoid(output[0, b * sz_hwa + ind]) + grid_x[b * sz_hwa + ind]
            ys_inb = torch.sigmoid(output[1, b * sz_hwa + ind]) + grid_y[b * sz_hwa + ind]
            ws_inb = torch.exp(output[2, b * sz_hwa + ind]) * anchor_w[b * sz_hwa + ind]
            hs_inb = torch.exp(output[3, b * sz_hwa + ind]) * anchor_h[b * sz_hwa + ind]

            bcx = xs_inb.item() / w
            bcy = ys_inb.item() / h
            bw = ws_inb.item() / w
            bh = hs_inb.item() / h

            xmin = bcx - bw / 2.0
            ymin = bcy - bh / 2.0
            xmax = xmin + bw
            ymax = ymin + bh

            results[total + b, :] = np.asarray([xmin * 640, xmax * 640, ymin * 360, ymax * 360])

        total += batch
    etime = time.time()

    storeResultsToXML(results, cdataset.imageNames, myXmlDir)
    write(imageNum, etime - stime, allTimeFile)

    print('evaluating miou....')
    miou = 0
    for i in os.listdir(myXmlDir):
        box_cal = ParseXml((myXmlDir) + i.split('.')[0]+".xml")
        box_gd = ParseXml((gdXmlDir) + i.split('.')[0]+".xml")
        # print(box_cal, box_gd)
        iou = GetIOU(box_cal, box_gd)
        miou += iou
    # print(miou)
    miou /= (len(os.listdir(myXmlDir)))
    print("pytorch iou", miou)



if __name__ == "__main__":
    main()
