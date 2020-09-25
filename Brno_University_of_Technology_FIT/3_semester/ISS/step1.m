% Author: Jan Ondruch - xondru14 	%
% Date:   6.11.2015					%

% matlab library fix
iptsetpref('UseIPPL', false);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	 Exercise1: Image Sharpening    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% loading the source image (src + copy - current instance of it)
img_src = imread('xondru14.bmp');
img_curr = imread('xondru14.bmp');

% matrix H_S for sharpening & sharpening process
H_S = [ -0.5 -0.5 -0.5; -0.5 5 -0.5; -0.5 -0.5 -0.5];
img_sharpened = imfilter(img_curr, H);

% output image & saving it
img_out = 'step1.bmp'
imwrite(img_sharpened, img_out);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	 Exercise2: Image Rotation      %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% rotating the image using fliplr function
img_rotated = fliplr(img_sharpened);

% output image & saving it
img_out = 'step2.bmp'
imwrite(img_rotated, img_out);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	 Exercise3: Median Filtering    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% application of median filter using medfilt2 function
img_median = medfilt2(img_rotated, [5 5]);

% output image & saving it
img_out = 'step3.bmp'
imwrite(img_median, img_out);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	 Exercise4: Image Blurring      %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% matrix H_B for blurring process & blurring using imfilter func
H_B = [ 1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
img_blurred = imfilter(img_median, H_B);

% output image & saving it
img_out = 'step4.bmp'
imwrite(img_median, img_out);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	 Exercise5: Flaw In Image       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% flip the picture back
img_flaw_tmp = fliplr(img_blurred);

% uint8 to double format conversion
img_flaw_tmp_dbl = im2double(img_flaw_tmp);	% edited picture
img_src_dbl = im2double(img_src);			% original picture

% getting K and L - horizontal and vertical values
img_src_size = size(img_src);
img_src_size_l = max(img_src_size);
img_src_size_k = min(img_src_size);


% go through the whole pics, compare 'em and save the diffs - flaw 
flaw = 0;

for (i = 1: img_src_size_k)
	for (j = 1: img_src_size_l)
		flaw = flaw + abs(img_src_dbl(i, j) - img_flaw_tmp_dbl(i, j));
	end;
end;

% count the final flaw
val = 255;
flaw = (flaw / (img_src_size_k * img_src_size_l)) * val


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Exercise6: Histogram Expansion   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% uint8 to double format conversion
img_blurred_dbl = im2double(img_blurred);

% find the min and the max value in the image
val_min = min(img_blurred_dbl);
val_max = max(img_blurred_dbl);

% get the values and set them accordingly
val_l = min(val_min);
val_k = max(val_max);
val_l = 0.0;
val_k = 1.0;

% linear re-mapping 
img_historam = imadjust(img_blurred, [val_l val_k], [val_l, val_k];

% output image & saving it
img_out = 'step5.bmp'
imwrite(img_historam, img_out);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Exercise7: Mid Val & Deviation   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% uint8 to double format conversion
img_historam_dbl = im2double(img_historam);


% deviation before histogram usage
midval_before = mean2(img_blurred_dbl) * val
dev_before = std2(img_blurred_dbl) * val

% deviation after histogram usage
midval_after = mean2(img_historam_dbl) * val
dev_after = std2(img_historam_dbl) * val


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Exercise8: Quantization          %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

N = 2;	% bit count
a = 0;
b = 255;

img_temp = double(img_historam);


% nulling all pixels for the final result of quantification
res_q = zeros(img_src_size_k, img_src_size_l);

% function for quantization - we use rounding 
for (i = 1: img_src_size_k)
	for (j = 1: img_src_size_l)
		res_q(i, j) = round (((2^N) - 1) * (img_temp(i, j) - a) / (b - a)) * (b - a) / ((2^N) - 1) + a;
	end;
end;


res_q2 = uint8(res_q);

% output image & saving it
img_out = 'step6.bmp';
imwrite(res_q2, img_out);