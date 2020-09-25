import { Pipe, PipeTransform } from '@angular/core';

@Pipe({name: 'sort'})
export class ArraySortPipe implements PipeTransform {
  transform(array: any[], field: string, order :string, ei: number): any[] {
    if (array[0].exercises !== undefined && ei !== -1) {
      array.sort((a: any, b: any) => {
        if (a.exercises[ei][field] < b.exercises[ei][field]) {
          return (order === "descend") ? -1: 1;
        } else if (a.exercises[ei][field] > b.exercises[ei][field]) {
          return (order === "descend") ? 1 : -1;
        } else {
          return 0;
        }
      });
    } else {
      array.sort((a: any, b: any) => {
        if (a[field] < b[field]) {
          return (order === "descend") ? -1: 1;
        } else if (a[field] > b[field]) {
          return (order === "descend") ? 1 : -1;
        } else {
          return 0;
        }
      });
    }
    return array;
  }
}