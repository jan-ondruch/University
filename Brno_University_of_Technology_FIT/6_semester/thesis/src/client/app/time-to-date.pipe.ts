import { Pipe, PipeTransform } from '@angular/core';

@Pipe({name: 'timeToDate'})
export class TimeToDatePipe implements PipeTransform {
  transform(count: number, type: string): any {
    if (type === "seconds")
      return new Date(1970, 0, 1).setSeconds(count);
    else 
      return new Date(1970, 0, 1).setMilliseconds(count);
  }
}